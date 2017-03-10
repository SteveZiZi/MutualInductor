
#include <iostream>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "fpga.hpp"
#include <unistd.h>
#include "raw_wave.hpp"

#include "fpga_reg.h"
#include "ac_source.h"
#include "adc_ctrl.h"
#include "dc_source.h"


#define FPGA_DEV_VERSION  "0.7.01"


#define STEABY_OUTPUT_OFFSET    0.0005
#define STEABY_OUTPUT_PERCENT   0.002


extern "C" {
    void fpga_reset( void );
    int open_smc(const char* dev);
    int close_smc(int fd);

    int start_sample(void);
    int stop_sample(void);
}

fpga_dev::fpga_dev() :
    acSource(0),
    adcCtrl(0),
    dcSource(0),
    m_sourceType(AC_SOURCE_OUTPUT)
{
    this->fd = -1;

    this->fd = open_smc(0);
    assert(this->fd >= 0 );

    printf("==fpga_dev version: %s built %s at %s=\n", FPGA_DEV_VERSION, __DATE__, __TIME__);
}

fpga_dev::~fpga_dev()
{
    if (this->fd >= 0 ){
        close_smc(this->fd);
        this->fd = -1;
    }

    if (acSource)
        delete acSource;
    if (adcCtrl)
        delete adcCtrl;
    if (dcSource)
        delete dcSource;
}



void fpga_dev::init( int cycle_count )
{
    printf("====fpga_dev::init=====\n");

    // reset FPGA
    fpga_reset();
    CT_LOG(("fpga boot version=0x%x\n", READ_THIS_REG(fpga_version) ));

    this->set_ac_source_output();
    ::reset_error_code();

    acSource = new ac_source(this);
    adcCtrl  = new adc_ctrl();
    dcSource = new dc_source();
    this->stop_output();

    this->cycleCount = cycle_count;

    WRITE_THIS_REG(signal_filter_times, (uint32_t)(120000));  // 1ms
}

void fpga_dev::deinit()
{

}

void fpga_dev::reset()
{
    this->safe_output(0, false);
    this->set_ac_source_output();

    ::reset_error_code();
    WRITE_THIS_REG(signal_filter_times, (uint32_t)(120000));  // 1ms
    this->clear_overflow_status(3);
    ::set_error_code(ERR_SUCCESS);

    this->dc_ac_f_set(50.0);
    this->setCycleCount(this->cycleCount);
    this->set_np_per_cycle(this->adcCtrl->getNpPerCycle());
}

bool fpga_dev::adc_start_sample()
{
    adcCtrl->setCycleCount(this->cycleCount);
    return adcCtrl->start_adc();
}
bool fpga_dev::adc_stop_sample()
{
    return adcCtrl->stop_adc();
}
bool fpga_dev::is_sample_started()
{
    return adcCtrl->is_adc_started();
}

int fpga_dev::set_max_range()
{
    select_range_by_value(LOAD_CUR_CHNL,   100.0);
    select_range_by_value(SRC_VOL_CHNL,    300.0);
    select_range_by_value(PRI_VOL_CHNL,    300.0);
    select_range_by_value(SECOND_VOL_CHNL, 300.0);

    return 0;
}

bool fpga_dev::select_range_by_value(int chnl, double peakVal)
{
    return adcCtrl->select_range_by_value(chnl, peakVal);
}

bool fpga_dev::isContinueSample()
{
    return adcCtrl->isContinueSample();
}
void fpga_dev::setContinueSample(bool enabled)
{
    adcCtrl->setContinueSample(enabled);
}

void fpga_dev::convToRealRange()
{
    adcCtrl->convToRealRange();
}


double fpga_dev::get_load_current()
{
    return adcCtrl->get_load_current();
}
double fpga_dev::get_output_vol()
{
    return adcCtrl->get_output_vol();
}
double fpga_dev::get_load_vol()
{
    return adcCtrl->get_load_vol()*50.0/adcCtrl->getFreq();
}
double fpga_dev::get_pri_vol()
{
    return adcCtrl->get_pri_vol();
}

double fpga_dev::get_freq()
{
    return adcCtrl->getFreq();
}

bool fpga_dev::is_used_dc_12v()
{
    return acSource->is_used_dc_12v();
}

int fpga_dev::get_indicate_sample_wave(raw_wave *wave_i_load, raw_wave *wave_u_src, raw_wave *wave_u_load, raw_wave *wave_u_pri)
{
    if (acSource->pidCtrlUsed())
        return acSource->get_indicate_sample_wave(wave_i_load, wave_u_src, wave_u_load, wave_u_pri);
    else
        return get_active_sample_wave(wave_i_load, wave_u_src, wave_u_load, wave_u_pri);
}

void fpga_dev::startRecord()
{
    adcCtrl->startRecord();
}

void fpga_dev::stopRecord()
{
    adcCtrl->stopRecord();
}

void fpga_dev::recordWave(std::vector<double> &vol, std::vector<double> &cur)
{
    adcCtrl->recordWave(vol, cur);
}

std::string fpga_dev::fpga_version(void)
{
    uint32_t ver = READ_THIS_REG(fpga_version);

    ver = ver % 1000;
    std::string str_ver;
    str_ver += ver/100+'0';
    str_ver += '.';
    char tmp[8] = {0};
    sprintf(tmp, "%2X", ver%100);
    str_ver.append(tmp);

    return str_ver;
}

std::string fpga_dev::dev_version(void)
{
    std::string str_ver;
    str_ver = FPGA_DEV_VERSION;
    return str_ver;
}

void fpga_dev::change_to_ac_source()
{
    if (is_dc_source_output()) {
        dcSource->demagnetization();
    }
    set_ac_source_output();
}

void fpga_dev::change_to_dc_source()
{
    if (is_ac_source_output()) {
        acSource->demagnetization();
    }
    set_dc_source_output();
}

/*
 *  @Func:  开始电源模块输出
 *
 *  @Return:
 *      0: success
 *     <0: failed
 *  @Note:
 *     1、电源模块的输出时能，输出电压为0V。仅需要调用一次，后续直接调用set_v配置输出电压即可。
 *     2、调用stop_output函数停止电源模块输出
 */
int fpga_dev::start_output()
{
    if (is_ac_source_output())
        return acSource->startOutput();
    return dcSource->startOutput();
}

/*
 *  @Func:  停止电源模块输出
 *
 *  @Return:
 *      0: success
 *     <0: failed
 *  @Note:
 *     1、停止电源的输出，在停止前会配置电源模块输出到0V。
 */
int fpga_dev::stop_output()
{
    if (is_ac_source_output())
        return acSource->stopOutput();
    return dcSource->stopOutput();
}

bool fpga_dev::is_output_started()
{
    if (is_ac_source_output())
        return acSource->outputStarted();
    return dcSource->outputStarted();
}

/*
 *  @Func:  safe_output
 *  @Desc:
 *     电源的输出不能跳变，只能平稳过度，配置电源安全输出到指定电压
 *  @Input:
 *     v:               output voltage
     *     wait_steaby:     wait output steaby
 *
 *  @Return:
 *      0: success
 *     <0: failed
 *  @Note:
 */
int fpga_dev::safe_output(double v, bool wait_steaby)
{
    if (!this->is_output_started())
        return -1;

    if (::get_error_code()) {
        printf("fpga_dev::safe_output error has happend:errCode(0x%x)\n", ::get_error_code());
        this->adc_stop_sample();
        demagnetization();
        return -1;
    }

    int ret = this->output_vol(v);

    bool adc_started = this->is_sample_started();
    if (wait_steaby && adc_started && ret>=0)
        wait_output_steaby();

    return ret;
}

int fpga_dev::output_vol(double v)
{
    if (is_ac_source_output())
        return acSource->set_v(v);
    return dcSource->safe_output(v);
}

bool fpga_dev::wait_output_steaby(AD_CHNL_STYLE /*chnl*/ )
{
    raw_wave src;
    double vol = 0.01;
    int ncnt = 0;
    bool ok = false;

    do {
        if (0 > get_active_sample_wave(0, &src, 0, 0)) {
            break;
        }
        if (!is_output_started()) {
            break;
        }
        src.calculate_rms();

        if (acSource->is_used_dc_12v()) {
            if (fabs(src.get_rms()-vol) < STEABY_OUTPUT_OFFSET)
                ok = true;
        }
        else {
            if ((fabs(src.get_rms()-vol)/vol) < STEABY_OUTPUT_PERCENT)
                ok = true;
        }
        vol = src.get_rms();
        ncnt++;
    }while(!ok&&ncnt<100);

    return ok;
}

/*
 *  @Func:  退磁
 *
 *  @input:
 *      none
 *
 *  @Return:
 *      0: success
 *     -1: failed
 *
 *  @Note:
 *      测试结束后，必须对CT/PT退磁操作
 */
int fpga_dev::demagnetization()
{
    adcCtrl->stop_adc();

    if (is_ac_source_output())
        return acSource->demagnetization();
    return dcSource->demagnetization();
}


/*
 *  @Func:  get_active_sample_wave
 *
 *  @Desc:
 *
 *  @input:
 *      none
 *  @Return:
 *      0: success
 *     -1: failed
 *  @Note:
 *      adaptive ADC range for LOAD_CUR_CHNL and PRI_VOL_CHNL
 */
int fpga_dev::get_active_sample_wave(raw_wave *wave_i_load,
                                     raw_wave *wave_u_src,
                                     raw_wave *wave_u_load,
                                     raw_wave *wave_u_pri)
{
    return adcCtrl->get_active_sample_wave(wave_i_load, wave_u_src, wave_u_load, wave_u_pri);

}

const adc_range *fpga_dev::adc_range_info(AD_CHNL_STYLE chnl)
{
    return adcCtrl->adc_range_info(chnl);
}


int fpga_dev::dc_ac_f_set(double freq )
{
    int ret = acSource->dc_ac_f_set(freq);
    adcCtrl->setFreq(freq);
    return ret;
}

int fpga_dev::set_np_per_cycle( int np_per_cycle )
{
    return adcCtrl->setNpPerCycle(np_per_cycle);
}

int fpga_dev::setCycleCount(int cycle_count)
{
    this->cycleCount = cycle_count;
    adcCtrl->setCycleCount(cycle_count);
    return 0;
}

//忽略AD的采样值
void fpga_dev::ignoreADSampleValue(int cnt)
{
    for(int i = 0; i < cnt; i++)
        this->read(0, 0, 0, 0);
}

/*
 *  @Func:  read
 *  @Desc:
 *      从FPGA获取4通道AD的采样数据
 *  @Return:
 *      0: success
 *     -1: failed
 *  @Note:
 *      测试结束后，必须对CT/PT退磁操作
 *      在量程切换后，驱动的缓冲区包含了切换前后的AD采样值，应当忽略掉。
 */
int fpga_dev::read( raw_wave *wave_i_load,
                    raw_wave *wave_u_src,
                    raw_wave *wave_u_load,
                    raw_wave *wave_u_pri )
{
    return adcCtrl->read(wave_i_load, wave_u_src, wave_u_load, wave_u_pri);
}

/*
 * 设置交流电源输出
 */
void fpga_dev::set_ac_source_output()
{
    WRITE_THIS_REG( output_sel, uint32_t(AC_SOURCE_OUTPUT) );
    m_sourceType = AC_SOURCE_OUTPUT;
}

/*
 * 设置直流电源输出，用于直阻的测试
 */
void fpga_dev::set_dc_source_output()
{
    WRITE_THIS_REG( output_sel, uint32_t(DC_SOURCE_OUTPUT) );
    m_sourceType = DC_SOURCE_OUTPUT;
}
bool fpga_dev::is_ac_source_output()
{
    return (m_sourceType == AC_SOURCE_OUTPUT) ? true : false;
}
bool fpga_dev::is_dc_source_output()
{
    return (m_sourceType == DC_SOURCE_OUTPUT) ? true : false;
}


bool fpga_dev::is_current_overflow(void)
{
    return adcCtrl->is_current_overflow();
}

bool fpga_dev::is_temperature_overflow()
{
    return adcCtrl->is_temperature_overflow();
}

void fpga_dev::clear_overflow_status(int flag)
{
    adcCtrl->clear_overflow_status(flag);
}


#include "src/inc/tmutex.h"
#include "src/inc/auto_guard.h"
//declared in common.h
class error_mgr
{
public:
    error_mgr():err_code(0)
    {
    }
    ~error_mgr()
    {
    }

    void set_error_code(uint32_t flag) {AUTO_GUARD(guard, ThreadMutex, tMutex); err_code |= flag;}
    uint32_t get_error_code() {AUTO_GUARD(guard, ThreadMutex, tMutex); return err_code;}
    void reset_error_code() {AUTO_GUARD(guard, ThreadMutex, tMutex); err_code = NO_ERR;}

private:
    uint32_t err_code;
    struct ThreadMutex tMutex;
};

static error_mgr s_err_mgr;

void set_error_code(uint32_t flag)
{
    s_err_mgr.set_error_code(flag);
}

uint32_t get_error_code()
{
    return s_err_mgr.get_error_code();
}

void reset_error_code()
{
    s_err_mgr.reset_error_code();
}

std::string error_code_to_string(uint32_t err_code)
{
    std::string strErr = "";

    if ( (err_code&ERR_SRC_VOL_AD) ||
         (err_code&ERR_LOAD_CUR_AD) ||
         (err_code&ERR_LOAD_VOL_AD))
    {
        strErr += "采集板卡A异常\n";
    }

    if ((err_code&ERR_PRI_VOL_AD) || (err_code&ERR_DC_SOURCE))
    {
        strErr += "采集板卡B异常\n";
    }

    if (err_code&ERR_AC_SOURCE) {
        strErr += "电源板异常\n";
    }


    return strErr;
}
