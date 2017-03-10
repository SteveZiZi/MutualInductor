#include "adc_ctrl.h"
#include "fpga_reg.h"
#include "private_def.h"
#include "raw_wave.hpp"
#include "src/inc/auto_guard.h"
#include "src/inc/tmutex.h"
#include "adcCalibration.h"
#include "src/inc/highprecisiontimecounter.h"

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>


#define EN_CALIBRATE_RANGE 0

enum {
    ADC_STOPPED              = 0,
    ADC_STARTED              = 1<<0,
    ADC_ACQUIRE_DATA         = 1<<1,

    // set cmd
    ADC_SET_MASK = 0xFFFF0000,
    ADC_SET_SAMPLE_POINTS    = 1<<16,   // sample points per cycle
    ADC_SET_CYCLES           = 1<<17,   // cycles for once read adc
    ADC_SET_SIGNAL_REQ       = 1<<18,
};



#define FPGA_BUFFER_SIZE    (16384+4)
#define AD_CHNL_BUFFER_SIZE (4096)

extern "C" {
    void fpga_reset( void );
    int read_sample_buf(unsigned char* buf, int count);
    int start_sample(void);
    int stop_sample(void);
}

static const struct adc_range g_tc[] = {
    {0x00, -0.01*1.0*0.8,      0.008*0.8,       10/(32768*0.505*10*41*6),    0.00},
    {0x08,  0.008*0.8*0.8,     0.400*0.8,       10/(32768*0.505*10*1*6),     0.00},
    {0x01,  0.400*0.8*0.8,     1.000*0.8,       10/(32768*0.005*10*41*6),    0.00},
    {0x09,  1.000*0.8*0.8,     100.0*0.8,       10/(32768*0.005*10*1*6),     0.00}
};
range_factor g_tc_factor[4] =
{
    {1.00,   0.00},
    {1.00,   0.00},
    {1.00,   0.00},
    {1.00,   0.00}
};

static const struct adc_range g_tv_src[] = {
    {0x01,  -0.1*0.8*0.8,      0.85*0.8,       10 / (32768.0*26/25*11),        0.0},
    {0x03,   0.85*0.8*0.8,     9.61*0.8,       10 / (32768.0*26/25*1),         0.0},
    {0x00,   9.61*0.8*0.8,     22.7*0.8,       10 / (32768.0*1/25*11),         0.0},
    {0x02,   22.7*0.8*0.8,     500/*250*/*0.8, 10 / (32768.0*1/25*1),          0.0},
};
range_factor g_tv_src_factor[4] =
{
    {1.00,  0.00},
    {1.00,  0.00},
    {1.00,  0.00},
    {1.00,  0.00}
};

static const struct adc_range g_tv_sec[] = {
    {0x01,  -0.1*0.8*0.8,      0.85*0.8,       10 / (32768.0*26/25*11),         0.0},
    {0x03,   0.85*0.8*0.8,     9.61*0.8,       10 / (32768.0*26/25*1),          0.0},
    {0x00,   9.61*0.8*0.8,     22.7*0.8,       10 / (32768.0*1/25*11),          0.0},
    {0x02,   22.7*0.8*0.8,     500/*250*/*0.8, 10 / (32768.0*1/25*1),           0.0},
};
range_factor g_tv_sec_factor[4] =
{
    {1.00,  0.00},
    {1.00,  0.00},
    {1.00,  0.00},
    {1.00,  0.00}
};

static const struct adc_range g_tv_pri[] = {
    {0x01,  -0.1*0.8*0.8,      0.041*0.8,      10 / (32768.0*22*11),    0.0},
    {0x03,   0.041*0.8*0.8,    0.455*0.8,      10 / (32768.0*22*1),     0.0},
    {0x00,   0.455*0.8*0.8,    4.55*0.8,       10 / (32768.0*1/5*11),   0.0},
    {0x02,   4.55*0.8*0.8,     500/*250*/*0.8, 10 / (32768.0*1/5*1),    0.0},
};
range_factor g_tv_pri_factor[4] =
{
    {1.00,  0.00},
    {1.00,  0.00},
    {1.00,  0.00},
    {1.00,  0.00}
};

bool is_outoff_range(double v, const struct adc_range *range)
{
   return (v < range->range_min || v > range->range_max);
}

bool get_adc_ranges(int chnl, const adc_range* *ranges, range_factor* *factors, int* arrSize)
{
    if (chnl >= MAX_CHNL_NUM || ranges==NULL || factors==NULL || arrSize==NULL)
        return false;

    switch(chnl) {
    case LOAD_CUR_CHNL:
        *ranges = g_tc;
        *factors = g_tc_factor;
        *arrSize = sizeof(g_tc)/sizeof(g_tc[0]);
        break;
    case SRC_VOL_CHNL:
        *ranges = g_tv_src;
        *factors = g_tv_src_factor;
        *arrSize = sizeof(g_tv_src)/sizeof(g_tv_src[0]);
        break;
    case LOAD_VOL_CHNL:
        *ranges = g_tv_sec;
        *factors = g_tv_sec_factor;
        *arrSize = sizeof(g_tv_sec)/sizeof(g_tv_sec[0]);
        break;
    case PRI_VOL_CHNL:
        *ranges = g_tv_pri;
        *factors = g_tv_pri_factor;
        *arrSize = sizeof(g_tv_pri)/sizeof(g_tv_pri[0]);
        break;
    default:
        break;
    }

    return true;
}

class adc_ctrl_private
{
    friend class adc_ctrl;
public:
    adc_ctrl_private(adc_ctrl *ctrl);
    ~adc_ctrl_private();

    bool start_adc();
    void stop_adc();
    bool is_started() {AUTO_GUARD(gd, ThreadMutex, m_mutex); return adcStarted;}

    int getCycleCount() const {return this->cycleCount;}
    void setCycleCount(int value);

    int getNpPerCycle() const {return npPerCycle;}
    int setNpPerCycle(int value) {
        this->npPerCycle = value;
        return config();
    }

    double getFreq() const {return freq;}
    void setFreq(double value) {
        freq = value;
        config();
    }

    int  read_adc_adjust_range(raw_wave *wave_i_load,
                               raw_wave *wave_u_src,
                               raw_wave *wave_u_load,
                               raw_wave *wave_u_pri);

    int read( raw_wave *wave_i_load,
              raw_wave *wave_u_src,
              raw_wave *wave_u_load,
              raw_wave *wave_u_pri);

    bool select_range_by_value( int chnl, double peak );

    double get_load_current() {
        AUTO_GUARD(gd, ThreadMutex, m_mutex);
        return this->current;
    }
    double get_output_vol() {
        AUTO_GUARD(gd, ThreadMutex, m_mutex);
        return this->voltage[0];
    }
    double get_load_vol() {
        AUTO_GUARD(gd, ThreadMutex, m_mutex);
        return this->voltage[1];
    }
    double get_pri_vol() {
        AUTO_GUARD(gd, ThreadMutex, m_mutex);
        return this->voltage[2];
    }

    void setMaxRange();

    bool is_current_overflow(void);
    bool is_temperature_overflow();
    void clear_overflow_status(int flag);

    void getCalibrationFactor();
    void saveCalibrationFactor();
    void convToRealRange();

    void startRecord();
    void stopRecord();
    void recordWave(std::vector<double> &vol, std::vector<double> &cur);


private:
    static void adc_sample_thread(adc_ctrl_private *p_this);
    void set_sample_points();
    void set_cycle_count();
    void set_freq();

    void update_fs(void) {
        this->fs = this->freq * this->npPerCycle;
    }

    int config();


private:
    sem_t sem;
    int cycleCount;
    int npPerCycle;
    double fs;
    double freq;
    union range_set regRangeSet;
    int16_t *adData;
    bool adcStarted;
    raw_wave *rawWave[MAX_CHNL_NUM];
    const struct adc_range *adc_range[MAX_CHNL_NUM];

    uint32_t adc_run_status;

    bool continue_sample;
    struct ThreadMutex m_mutex;
    double current;
    double voltage[3];

    const ad_calibrate_result *m_adcCalibrateResult[MAX_CHNL_NUM];
    struct adc_range m_adcRange[MAX_CHNL_NUM][4];

    adc_ctrl* adcCtrl;
    bool killThread;

    std::vector<double> m_volRecord;
    std::vector<double> m_curRecord;
    bool m_bStartRecord;

};

adc_ctrl_private::adc_ctrl_private(adc_ctrl *ctrl) :
    cycleCount(0),
    adcStarted(false),
    adc_run_status(ADC_STOPPED),
    continue_sample(true),
    adcCtrl(ctrl),
    killThread(false),
    m_bStartRecord(false)
{
    getCalibrationFactor();
    convToRealRange();

    regRangeSet.value = 0;
    this->current = 0.0;
    this->voltage[0] = this->voltage[1] = this->voltage[2] = 0.0;

    for(int i = 0; i < MAX_CHNL_NUM; i++)
        rawWave[i] = 0;

    this->adData = new int16_t[FPGA_BUFFER_SIZE/2];

    this->setMaxRange();
    this->setCycleCount(2);
    this->setNpPerCycle(1024);
    this->setFreq(50.0);
    this->clear_overflow_status(3);

    sem_init(&this->sem, 0, 0);
    pthread_t tid = 0;
    pthread_create( &tid, NULL, (void* (*)(void*))adc_sample_thread, this);
    std::cout << "adc_ctrl_private tid ="<< tid << std::endl;
    assert(tid);
}

adc_ctrl_private::~adc_ctrl_private()
{
    if (this->adData){
        delete[] this->adData;
    }
    sem_destroy(&this->sem);
    killThread = true;
    usleep(50000);
}

void adc_ctrl_private::setCycleCount(int value)
{
    std::cout <<  __func__ << " cycle_count:" << value << std::endl;
    if (this->cycleCount != value) {
        cycleCount = value;
    }
}

void adc_ctrl_private::set_sample_points()
{
    this->setNpPerCycle(adcCtrl->np_per_cycle);
}
void adc_ctrl_private::set_cycle_count()
{
    this->setCycleCount(adcCtrl->cycle_count);
}
void adc_ctrl_private::set_freq()
{
    this->setFreq(adcCtrl->freq);
}

int adc_ctrl_private::config()
{
    int v = 7;
    switch(npPerCycle) {
    case 32   : v = 0;break;
    case 64   : v = 1;break;
    case 128  : v = 2;break;
    case 256  : v = 3;break;
    case 512  : v = 4;break;
    case 1024 : v = 5;break;
    case 2048 : v = 6;break;
    case 4096 : v = 7;break;
    case 8192 : v = 8;break;
    default:
        printf("np per cycle setting error\n");
        return -1;
    }
    this->update_fs();
    CT_LOG(("np_per_cycle:%d freq:%f fs=%f\n", npPerCycle, freq, this->fs));

    uint64_t reg = 100000000ULL*this->npPerCycle/this->fs;
    if ( 0xffffffff00000000ULL & reg ) {
        printf("set sample rate overfllow\n");
        return -1;
    }

    union ping_config config;
    config.value = 0;
    config.fix_follow_sel  = 1; // 固定
    config.sample_rate_set = v;
    config.frame_length    = 128;

    WRITE_THIS_REG(fix_sample_count, (uint32_t)reg);
    return WRITE_THIS_REG(ping_config, config.value);
}

int adc_ctrl_private::read_adc_adjust_range(raw_wave *wave_i_load,
                                            raw_wave *wave_u_src,
                                            raw_wave *wave_u_load,
                                            raw_wave *wave_u_pri)
{
    if (!this->is_started()) {
        return -1;
    }
    raw_wave *waves[MAX_CHNL_NUM] = {wave_i_load, wave_u_src, wave_u_load, wave_u_pri};
    int tryCnt = 0;

readAgain:
    if (-1 == this->read(wave_i_load, wave_u_src, wave_u_load, wave_u_pri)) {
        return -1;
    }
    if (this->adc_run_status == ADC_STOPPED) {
        this->setMaxRange();
        return 0;
    }
    int flag = 0;
    for(int i = 0; i < MAX_CHNL_NUM; i++)
    {
        if (waves[i] == 0) {
            continue;
        }

        waves[i]->calculate_peak();
        double peak = waves[i]->get_peek();
        if (is_outoff_range(peak, this->adc_range[i])) {
            this->select_range_by_value(i, peak);
            flag = 1;
        }
    }
    if (waves[LOAD_CUR_CHNL]->get_peek()*waves[SRC_VOL_CHNL]->get_peek() > 1000.0)
        return -3;

    if (flag) {
        for(int i = 0; i < 2; i++) this->read(0, 0, 0, 0);
        tryCnt++;
        if (tryCnt < 50) {
            goto readAgain;
        }
        else
            return -2;
    }

    return 0;
}

bool adc_ctrl_private::select_range_by_value(int chnl, double peak)
{
    const struct adc_range *p_range = 0;
    double value = fabs(peak);

    if (chnl<0 || chnl >= MAX_CHNL_NUM){
        CT_LOG(("select range_by value ch NOT in range. ch=%d\n", chnl));
        return false;
    }

    int n = sizeof(m_adcRange[chnl])/sizeof(m_adcRange[chnl][0]);
    for (int i=0; i<n; i++) {
        if ( value >= m_adcRange[chnl][i].range_min && value <= m_adcRange[chnl][i].range_max ){
            p_range = &m_adcRange[chnl][i];
        }
    }
    if ( value >= m_adcRange[chnl][n-1].range_max ){
        p_range = &m_adcRange[chnl][n-1];
    }

    {
        #ifndef NO_LOG
        const char *str_chnl_name[4] ={
            "Iload",
            "Usrc ",
            "Uload",
            "Upri",
        };
        #endif
        if (p_range) {
            CT_LOG(("select_ch_range: <%s>: found range: %02XH(%f -> %f) scale:%f by %f\n",
                    str_chnl_name[chnl],
                    p_range->fpga_reg_value,
                    p_range->range_min,
                    p_range->range_max,
                    p_range->scale,
                    value
                       ));
        } else {
            CT_LOG(("select_ch_range: <%s>: NOT found range by %f\n", str_chnl_name[chnl], value ));
            return false;
        }
    }

    this->adc_range[chnl] = p_range;

    switch (chnl) {
    case 0:
        this->regRangeSet.ch1_range_value = p_range->fpga_reg_value;
        break;
    case 1:
        this->regRangeSet.ch2_range_value = p_range->fpga_reg_value;
        break;
    case 2:
        this->regRangeSet.ch3_range_value = p_range->fpga_reg_value;
        break;
    case 3:
        this->regRangeSet.ch4_range_value = p_range->fpga_reg_value;
        break;
    }

    if ( WRITE_THIS_REG(range_set, regRangeSet.value) == 0 ) {
        return true;
    }
    return false;
}

void adc_ctrl_private::setMaxRange()
{
    this->select_range_by_value(LOAD_CUR_CHNL,   100.0);
    this->select_range_by_value(SRC_VOL_CHNL,    300.0);
    this->select_range_by_value(PRI_VOL_CHNL,    300.0);
    this->select_range_by_value(SECOND_VOL_CHNL, 300.0);
}

bool adc_ctrl_private::is_current_overflow()
{
    uint32_t value = READ_THIS_REG(dc_ac_run_status);

    if ( value & 0x01 ) {
        return true;
    }
    return false;
}
bool adc_ctrl_private::is_temperature_overflow()
{
    uint32_t value = READ_THIS_REG(dc_ac_run_status);

    if ( value & 0x02 ) {
        return true;
    }
    return false;
}
void adc_ctrl_private::clear_overflow_status(int flag)
{
    uint32_t value = READ_THIS_REG(dc_ac_run_status);
    value &= flag;
    WRITE_THIS_REG(dc_ac_run_status, value);
}


void adc_ctrl_private::getCalibrationFactor()
{
    CAdcCalibration* calibrator = CAdcCalibration::getInstance();
    if (calibrator->init() < 0) {
        saveCalibrationFactor();
    }
    else {
#if !EN_CALIBRATE_RANGE
        const ad_calibrate_result* result = calibrator->result(LOAD_CUR_CHNL);
        for(uint32_t i = 0; i < result->numOfRange; i++) {
            g_tc_factor[i].k = result->factors[i].k;
            g_tc_factor[i].b = result->factors[i].b;
        }
        result = calibrator->result(SRC_VOL_CHNL);
        for(uint32_t i = 0; i < result->numOfRange; i++) {
            g_tv_src_factor[i].k = result->factors[i].k;
            g_tv_src_factor[i].b = result->factors[i].b;
        }
        result = calibrator->result(LOAD_VOL_CHNL);
        for(uint32_t i = 0; i < result->numOfRange; i++) {
            g_tv_sec_factor[i].k = result->factors[i].k;
            g_tv_sec_factor[i].b = result->factors[i].b;
        }
        result = calibrator->result(PRI_VOL_CHNL);
        for(uint32_t i = 0; i < result->numOfRange; i++) {
            g_tv_pri_factor[i].k = result->factors[i].k;
            g_tv_pri_factor[i].b = result->factors[i].b;
        }
#endif
    }
#if EN_CALIBRATE_RANGE
    uint8_t *content = new uint8_t[256];
    ad_calibrate_result* result = reinterpret_cast<ad_calibrate_result*>(content);
    result->numOfRange = sizeof(g_tc_factor)/sizeof(g_tc_factor[0]);
    for(uint32_t i = 0; i < result->numOfRange; i++) {
        result->factors[i].k = g_tc_factor[i].k;
        result->factors[i].b = g_tc_factor[i].b;
    }
    calibrator->setResult(LOAD_CUR_CHNL, *result);

    result = reinterpret_cast<ad_calibrate_result*>(content);
    result->numOfRange = sizeof(g_tv_src_factor)/sizeof(g_tv_src_factor[0]);
    for(uint32_t i = 0; i < result->numOfRange; i++) {
        result->factors[i].k = g_tv_src_factor[i].k;
        result->factors[i].b = g_tv_src_factor[i].b;
    }
    calibrator->setResult(SRC_VOL_CHNL, *result);

    result->numOfRange = sizeof(g_tv_sec_factor)/sizeof(g_tv_sec_factor[0]);
    for(uint32_t i = 0; i < result->numOfRange; i++) {
        result->factors[i].k = g_tv_sec_factor[i].k;
        result->factors[i].b = g_tv_sec_factor[i].b;
    }
    calibrator->setResult(LOAD_VOL_CHNL, *result);

    result->numOfRange = sizeof(g_tv_pri_factor)/sizeof(g_tv_pri_factor[0]);
    for(uint32_t i = 0; i < result->numOfRange; i++) {
        result->factors[i].k = g_tv_pri_factor[i].k;
        result->factors[i].b = g_tv_pri_factor[i].b;
    }
    calibrator->setResult(PRI_VOL_CHNL, *result);

    delete[] content;
    //saveCalibrationFactor();
#endif

    calibrator->dumpRange();
    for(int i = 0; i < MAX_CHNL_NUM; i++)
        m_adcCalibrateResult[i] = calibrator->result(i);

}

void adc_ctrl_private::saveCalibrationFactor()
{
    std::cout << "++adc_ctrl_private::saveCalibrationFactor++" << std::endl;
    CAdcCalibration* calibrator = CAdcCalibration::getInstance();

    uint8_t *content = new uint8_t[256];
    ad_calibrate_result* result = reinterpret_cast<ad_calibrate_result*>(content);

    if (!calibrator->chnl_is_valid(LOAD_CUR_CHNL))
    {
        result->numOfRange = sizeof(g_tc_factor)/sizeof(g_tc_factor[0]);
        for(uint32_t i = 0; i < result->numOfRange; i++) {
            result->factors[i].k = g_tc_factor[i].k;
            result->factors[i].b = g_tc_factor[i].b;
        }
        calibrator->setResult(LOAD_CUR_CHNL, *result);
    }

    if (!calibrator->chnl_is_valid(SRC_VOL_CHNL))
    {
        result->numOfRange = sizeof(g_tv_src_factor)/sizeof(g_tv_src_factor[0]);
        for(uint32_t i = 0; i < result->numOfRange; i++) {
            result->factors[i].k = g_tv_src_factor[i].k;
            result->factors[i].b = g_tv_src_factor[i].b;
        }
        calibrator->setResult(SRC_VOL_CHNL, *result);
    }

    if (!calibrator->chnl_is_valid(LOAD_VOL_CHNL))
    {
        result->numOfRange = sizeof(g_tv_sec_factor)/sizeof(g_tv_sec_factor[0]);
        for(uint32_t i = 0; i < result->numOfRange; i++) {
            result->factors[i].k = g_tv_sec_factor[i].k;
            result->factors[i].b = g_tv_sec_factor[i].b;
        }
        calibrator->setResult(LOAD_VOL_CHNL, *result);
    }

    if (!calibrator->chnl_is_valid(PRI_VOL_CHNL))
    {
        result->numOfRange = sizeof(g_tv_pri_factor)/sizeof(g_tv_pri_factor[0]);
        for(uint32_t i = 0; i < result->numOfRange; i++) {
            result->factors[i].k = g_tv_pri_factor[i].k;
            result->factors[i].b = g_tv_pri_factor[i].b;
        }
        calibrator->setResult(PRI_VOL_CHNL, *result);
    }

    calibrator->saveResult();

    delete[] content;
}

static void _confToRealRange(adc_range *dst_range,
                             const adc_range *src_range,
                             const ad_calibrate_result *result)
{
    try {
        for(uint32_t n=0; n<4; n++) {
            range_factor factor = {1.0, 0.0};
            if (n < result->numOfRange) {
                factor.k = result->factors[n].k;
                factor.b = result->factors[n].b;
            }
            dst_range[n].fpga_reg_value = src_range[n].fpga_reg_value;
            dst_range[n].scale          = src_range[n].scale*factor.k;
            dst_range[n].line_offset    = src_range[n].line_offset+factor.b;
            dst_range[n].range_min      = src_range[n].range_min*factor.k-fabs(factor.b);
            dst_range[n].range_max      = src_range[n].range_max*factor.k-fabs(factor.b);

            if (n!=0) {
                if (dst_range[n].range_min > dst_range[n-1].range_max*0.8)
                    dst_range[n].range_min = dst_range[n-1].range_max*0.8;
            }
        }
    }
    catch(...) {
        printf("[ERR] _confToRealRange found exceptions\n");
    }
}
void adc_ctrl_private::convToRealRange()
{
    const ad_calibrate_result *result = m_adcCalibrateResult[LOAD_CUR_CHNL];
    _confToRealRange(m_adcRange[LOAD_CUR_CHNL], g_tc, result);

    result = m_adcCalibrateResult[SRC_VOL_CHNL];
    _confToRealRange(m_adcRange[SRC_VOL_CHNL], g_tv_src, result);

    result = m_adcCalibrateResult[LOAD_VOL_CHNL];
    _confToRealRange(m_adcRange[LOAD_VOL_CHNL], g_tv_sec, result);

    result = m_adcCalibrateResult[PRI_VOL_CHNL];
    _confToRealRange(m_adcRange[PRI_VOL_CHNL], g_tv_pri, result);
}

void adc_ctrl_private::startRecord()
{
    m_volRecord.clear();
    m_volRecord.reserve(1024*1024);
    m_curRecord.clear();
    m_curRecord.reserve(1024*1024);
    m_bStartRecord = true;
}

void adc_ctrl_private::stopRecord()
{
    m_bStartRecord = false;
}

void adc_ctrl_private::recordWave(std::vector<double> &vol, std::vector<double> &cur)
{
    vol = m_volRecord;
    cur = m_curRecord;
}

bool adc_ctrl_private::start_adc()
{
    if (!this->adcStarted)
        this->adcStarted = (start_sample() == 0) ? true : false;
    return this->adcStarted;
}

void adc_ctrl_private::stop_adc()
{
    stop_sample();
    this->adcStarted = false;
}

int adc_ctrl_private::read(raw_wave *wave_i_load, raw_wave *wave_u_src, raw_wave *wave_u_load, raw_wave *wave_u_pri)
{
    if (!this->is_started()) {
        return -1;
    }
    int m = 0;
    int to_np = AD_CHNL_BUFFER_SIZE/2;
    int np_need = to_np;
    int cycleCount = this->cycleCount;

    if (this->npPerCycle <= to_np) {
        cycleCount = to_np/this->npPerCycle;
        if (cycleCount < this->cycleCount) {
            m = this->cycleCount/cycleCount;
            cycleCount *= m;
        }
    }
    else {
        double cycleCountPerBuf = (double)(to_np)/(double)(this->npPerCycle);
        m = (int)(1.0/cycleCountPerBuf)*cycleCount;
    }

    raw_wave *waves[4] = { wave_i_load, wave_u_src, wave_u_load,  wave_u_pri};
    for(int chnl=0; chnl<4; chnl++) {
        if (waves[chnl]){
            waves[chnl]->set_channel_id(chnl);
            waves[chnl]->set_dynamic(cycleCount*this->npPerCycle, this->fs );
            waves[chnl]->set_freq( this->freq );
            waves[chnl]->set_np_per_cycle(this->npPerCycle);
        }
    }

    int i = 0;
    int offset = 0;
    do
    {
        if (FPGA_BUFFER_SIZE != read_sample_buf( (unsigned char*)this->adData, FPGA_BUFFER_SIZE ))
            return -1;
        for (int ch=0; ch<4; ch++) {
            int16_t *p  = this->adData + 1 + ch*(to_np);
            if (waves[ch])
            {
                waves[ch]->set_raw_data(p, offset, np_need,
                                        this->adc_range[ch]->scale,
                                        this->adc_range[ch]->line_offset);
            }
        }
        offset += np_need;
        i++;
    }while(i<m);

    return 0;
}



void adc_ctrl_private::adc_sample_thread(adc_ctrl_private *p_this)
{
    raw_wave wave_i_load;
    raw_wave wave_u_src;
    raw_wave wave_u_load;
    raw_wave wave_u_pri;

    bool bSetMaxRange = true;

    std::cout << __func__ << ": started" << std::endl;

    while(!p_this->killThread)
    {
        if (p_this->is_current_overflow()) {
            //::set_error_code(ERR_OVER_CURRENT);
            //p_this->clear_overflow_status(0x01);
            //printf("+++++++p_this->is_current_overflow++++++++++\n");
        }
//        if (p_this->is_temperature_overflow()) {
//            ::set_error_code(ERR_OVER_TEMP);
//        }

        if (p_this->adc_run_status&ADC_SET_MASK) {
            if (p_this->adc_run_status&ADC_SET_CYCLES) {
                p_this->set_cycle_count();
                p_this->adc_run_status &= ~ADC_SET_CYCLES;
            }
            if (p_this->adc_run_status&ADC_SET_SAMPLE_POINTS) {
                p_this->set_sample_points();
                p_this->adc_run_status &= ~ADC_SET_SAMPLE_POINTS;
            }
            if (p_this->adc_run_status&ADC_SET_SIGNAL_REQ) {
                p_this->set_freq();
                p_this->adc_run_status &= ~ADC_SET_SIGNAL_REQ;
            }
        }

        if (p_this->continue_sample && (p_this->adc_run_status&ADC_STARTED))
        {
            bSetMaxRange = false;
            if (0 > p_this->read_adc_adjust_range(&wave_i_load, &wave_u_src, &wave_u_load, &wave_u_pri)) {
                p_this->adc_run_status = ADC_STOPPED;
                ::set_error_code(ERR_SRC_VOL_AD|ERR_LOAD_CUR_AD|ERR_LOAD_VOL_AD|ERR_PRI_VOL_AD);
                printf("[ERR] adc_sample_thread: read adc failed.\n");
            }
            else {
                wave_i_load.calculate_rms();
                wave_u_src.calculate_rms();
                wave_u_load.calculate_rms();
                wave_u_pri.calculate_rms();

                AUTO_GUARD(gd, ThreadMutex, p_this->m_mutex);
                p_this->voltage[0] = wave_u_src.get_rms();
                p_this->voltage[1] = wave_u_load.get_rms();
                p_this->voltage[2] = wave_u_pri.get_rms();
                p_this->current = wave_i_load.get_rms();

                if (p_this->m_bStartRecord) {
                    const std::vector<double> &data = wave_u_src.get_readonly_buffer();
                    p_this->m_volRecord.insert(p_this->m_volRecord.end(), data.begin(), data.end());

                    const std::vector<double> &data2 = wave_i_load.get_readonly_buffer();
                    p_this->m_curRecord.insert(p_this->m_curRecord.end(), data2.begin(), data2.end());
                }
            }
            if (p_this->adc_run_status&ADC_ACQUIRE_DATA) {
                if (p_this->rawWave[LOAD_CUR_CHNL] != 0)
                    *p_this->rawWave[LOAD_CUR_CHNL] = wave_i_load;
                if (p_this->rawWave[SRC_VOL_CHNL] != 0)
                    *p_this->rawWave[SRC_VOL_CHNL] = wave_u_src;
                if (p_this->rawWave[LOAD_VOL_CHNL] != 0)
                    *p_this->rawWave[LOAD_VOL_CHNL] = wave_u_load;
                if (p_this->rawWave[PRI_VOL_CHNL] != 0)
                    *p_this->rawWave[PRI_VOL_CHNL] = wave_u_pri;
                p_this->adc_run_status &= ~ADC_ACQUIRE_DATA;
                sem_post(&p_this->sem);
            }
        }
        else {
            if (p_this->adc_run_status&ADC_ACQUIRE_DATA) {
                p_this->adc_run_status &= ~ADC_ACQUIRE_DATA;
                sem_post(&p_this->sem);
            }
            if (p_this->continue_sample && !bSetMaxRange) {
                p_this->setMaxRange();
                bSetMaxRange = true;
            }
            if (p_this->is_started()) {
                if (!(p_this->adc_run_status&ADC_STARTED))
                    p_this->stop_adc();
            }
            usleep(10000);
        }
    }

    std::cout << __func__ << ": exit " << std::endl;
}



/***********************************************************************
 * **************   adc_ctrl          **********************************
 * ********************************************************************/
adc_ctrl::adc_ctrl()
{
    _private = new adc_ctrl_private(this);
}

adc_ctrl::~adc_ctrl()
{
    delete _private;
}

int adc_ctrl::get_active_sample_wave(raw_wave *wave_i_load,
                                     raw_wave *wave_u_src,
                                     raw_wave *wave_u_load,
                                     raw_wave *wave_u_pri)
{
    if (!is_adc_started()) {
        return -1;
    }
    AUTO_GUARD(gd, ThreadMutex, this->m_mutex);
    if (_private->continue_sample) {
        this->_private->rawWave[LOAD_CUR_CHNL] = wave_i_load;
        this->_private->rawWave[SRC_VOL_CHNL]  = wave_u_src;
        this->_private->rawWave[LOAD_VOL_CHNL] = wave_u_load;
        this->_private->rawWave[PRI_VOL_CHNL]  = wave_u_pri;
        this->_private->adc_run_status |= ADC_ACQUIRE_DATA;
        sem_wait(&this->_private->sem);
        return (this->_private->adc_run_status==ADC_STOPPED)?-1:0;
    }
    else {
        return _private->read_adc_adjust_range(wave_i_load, wave_u_src, wave_u_load, wave_u_pri);
    }

    return 0;
}

int adc_ctrl::read(raw_wave *wave_i_load, raw_wave *wave_u_src, raw_wave *wave_u_load, raw_wave *wave_u_pri)
{
    if (_private->continue_sample) {
        return get_active_sample_wave(wave_i_load, wave_u_src, wave_u_load, wave_u_pri);
    }
    else
        return _private->read(wave_i_load, wave_u_src, wave_u_load, wave_u_pri);
}

bool adc_ctrl::start_adc()
{
    this->_private->adc_run_status = ADC_STARTED;
    return this->_private->start_adc();
}

bool adc_ctrl::stop_adc()
{
    this->_private->adc_run_status = ADC_STOPPED;
    return true;
}

bool adc_ctrl::is_adc_started()
{
    return ((_private->adc_run_status&ADC_STARTED) == ADC_STARTED)?true:false;
}

double adc_ctrl::get_load_current()
{
    return _private->get_load_current();
}
double adc_ctrl::get_output_vol()
{
    return _private->get_output_vol();
}
double adc_ctrl::get_load_vol()
{
    return _private->get_load_vol();
}
double adc_ctrl::get_pri_vol()
{
    return _private->get_pri_vol();
}

bool adc_ctrl::isContinueSample()
{
    return _private->continue_sample;
}
void adc_ctrl::setContinueSample(bool enabled)
{
    _private->continue_sample = enabled;
}

bool adc_ctrl::is_current_overflow()
{
    return _private->is_current_overflow();
}

bool adc_ctrl::is_temperature_overflow()
{
    return _private->is_temperature_overflow();
}

void adc_ctrl::clear_overflow_status(int flag)
{
    _private->clear_overflow_status(flag);
}

void adc_ctrl::startOverPowerProtect()
{
//    union range_set range = {0};
//    range.ch1_range_value = g_tc[sizeof(g_tc)/sizeof(g_tc[0])-1].fpga_reg_value;
//    range.ch2_range_value = g_tv_src[sizeof(g_tv_src)/sizeof(g_tv_src[0])-1].fpga_reg_value;
//    RITE_THIS_REG(overPowerRangeSet, value);
}

void adc_ctrl::clearOverPowerStatus()
{

}

void adc_ctrl::stopOverPowerProtect()
{

}

void adc_ctrl::convToRealRange()
{
    _private->convToRealRange();
}

void adc_ctrl::startRecord()
{
    _private->startRecord();
}

void adc_ctrl::stopRecord()
{
    _private->stopRecord();
}

void adc_ctrl::recordWave(std::vector<double> &vol, std::vector<double> &cur)
{
    _private->recordWave(vol, cur);
}

/*
 *  @Func:  select_range_by_value
 *
 *  @Desc:
 *      根据电压或电流值来选择AD相应的量程。
 *
 *  @input:
 *      chnl:     AD通道号 0:电流通道 1/2/3:电压通道
 *      peak:     峰值
 *
 *  @Return:
 *      true: success
 *     false: failed
 *
 *  @Note:
 *     在量程切换后，驱动的缓冲区包含了切换前后的AD采样值，应当忽略掉。
 */
bool adc_ctrl::select_range_by_value( int chnl, double peak )
{
    if (_private->continue_sample) {
        return false;
    }
    else
        return _private->select_range_by_value(chnl, peak);
}

const adc_range *adc_ctrl::adc_range_info(AD_CHNL_STYLE chnl)
{
    return _private->adc_range[chnl];
}

int adc_ctrl::getCycleCount() const
{
    return _private->getCycleCount();
}
void adc_ctrl::setCycleCount(int value)
{
    this->cycle_count = value;
    _private->adc_run_status |= ADC_SET_CYCLES;
}

int adc_ctrl::getNpPerCycle() const
{
    return _private->getNpPerCycle();
}
int adc_ctrl::setNpPerCycle(int value)
{
    this->np_per_cycle = value;
    _private->adc_run_status |= ADC_SET_SAMPLE_POINTS;
    return true;
}

double adc_ctrl::getFreq() const
{
    return _private->getFreq();
}
void adc_ctrl::setFreq(double value)
{
    this->freq = value;
    _private->adc_run_status |= ADC_SET_SIGNAL_REQ;
}
