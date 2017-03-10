
#ifndef FPGA_H
#define FPGA_H

#include <stdint.h>
#include <string>
#include <vector>

#include "private_def.h"


struct fpga_reg;
class raw_wave;

class ac_source;
class adc_ctrl;
class dc_source;


class fpga_dev
{
public:
    fpga_dev();
    virtual ~fpga_dev();


    enum {
        MAX_RANGE_VOL = 1000,
        MAX_RANGE_CUR = 35
    };

    void init( int cycleCount );
    void deinit(void);
    void reset(void);

    std::string fpga_version(void);
    std::string dev_version(void);

    void change_to_ac_source();
    void change_to_dc_source();
    bool is_ac_source_output();
    bool is_dc_source_output();

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
    int start_output();

    /*
     *  @Func:  停止电源模块输出
     *
     *  @Return:
     *      0: success
     *     <0: failed
     *  @Note:
     *     1、停止电源的输出，在停止前会配置电源模块输出到0V。
     */
    int stop_output();

    bool is_output_started();

    /*
     *  @Func:  safe_output
     *  @Desc:
     *     电源的输出不能跳变，只能平稳过度，配置电源安全输出到指定电压
     *  @Input:
     *     v:               output voltage
     *     wait_steaby:     wait output steaby
     *  @Return:
     *      0: success
     *     <0: failed
     *  @Note:
     */
    int safe_output(double v, bool wait_steaby = true);

    /*
     *  @Func:  退磁
     *
     *  @Return:
     *      0: success
     *     -1: failed
     *
     *  @Note:
     *      测试结束后，必须对CT/PT退磁操作
     */
    int demagnetization();


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
    int get_active_sample_wave(raw_wave *wave_i_load,
                               raw_wave *wave_u_src,
                               raw_wave *wave_u_load,
                               raw_wave *wave_u_pri);

    const struct adc_range* adc_range_info(AD_CHNL_STYLE chnl);

    bool adc_start_sample(void);
    bool adc_stop_sample(void);
    bool is_sample_started();
    int  set_max_range();
    bool select_range_by_value(int chnl, double peakVal);
    bool isContinueSample();
    void setContinueSample(bool enabled = true);

    void convToRealRange();


    double get_load_current();
    double get_output_vol();
    double get_load_vol();
    double get_pri_vol();
    double get_freq();

    bool is_used_dc_12v();
    bool is_used_dc_200V() {return !is_used_dc_12v();}

    //test
    int get_indicate_sample_wave(raw_wave *wave_i_load,
                                 raw_wave *wave_u_src,
                                 raw_wave *wave_u_load,
                                 raw_wave *wave_u_pri);

    void startRecord();
    void stopRecord();
    void recordWave(std::vector<double> &vol, std::vector<double> &cur);

public:
    int dc_ac_f_set( double freq ); // in Hz ; 4Hz ~ 300Hz
    /*
     *  设置一周波的采样点数
     */
    int set_np_per_cycle( int np_per_cycle );

    int setCycleCount(int cycleCount);
    int getCycleCount(void) const {return this->cycleCount;}

    bool wait_output_steaby(AD_CHNL_STYLE chnl = SRC_VOL_CHNL);

private:
    //忽略AD的采样值
    void ignoreADSampleValue(int cnt);

    /*
     *  @Func:  read
     *  @Desc:
     *      从FPGA获取4通道AD的采样数据
     *  @Return:
     *      0: success
     *     -1: failed
     *  @Note:
     *      在量程切换后，驱动的缓冲区包含了切换前后的AD采样值，应当忽略掉。
     */
    int read( raw_wave *wave_i_load,
              raw_wave *wave_u_src,
              raw_wave *wave_u_load,
              raw_wave *wave_u_pri);

    void set_ac_source_output();    //  设置交流电源输出
    void set_dc_source_output();    //  设置直流电源输出，用于直阻的测试

    // 运行状态exitDemo
    bool is_current_overflow(void);
    bool is_temperature_overflow(void);
    void clear_overflow_status(int flag);

    int output_vol(double v);   //

private:
    int fd;

    raw_wave *rawWave[MAX_CHNL_NUM];

    ac_source *acSource;
    adc_ctrl  *adcCtrl;
    dc_source *dcSource;


    int adc_chnl_flag;
    int cycleCount;

    int m_sourceType;

    friend class testAcSource;
    friend class CUserTest;
    friend class ctpt_ctrl_p;
    friend class testAdc;
    friend class settingsWidget;
};

#endif /* FPGA_H */
