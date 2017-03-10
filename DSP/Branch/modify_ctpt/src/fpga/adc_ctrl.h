#ifndef ADC_CTRL_H
#define ADC_CTRL_H

#include <pthread.h>
#include <semaphore.h>
#include "private_def.h"
#include "fpga_reg.h"
#include "src/inc/tmutex.h"
#include <vector>


class raw_wave;

class adc_ctrl_private;

class adc_ctrl
{
public:
    adc_ctrl();
    ~adc_ctrl();

    int get_active_sample_wave(raw_wave *wave_i_load,
                               raw_wave *wave_u_src,
                               raw_wave *wave_u_load,
                               raw_wave *wave_u_pri);

    int read( raw_wave *wave_i_load,
              raw_wave *wave_u_src,
              raw_wave *wave_u_load,
              raw_wave *wave_u_pri);
    /*
     *  @Func:  select_range_by_value
     *
     *  @Desc:
     *      根据电压或电流值来选择AD相应的量程。
     *
     *  @input:
     *      chnl:     AD通道号: 0:电流通道 1/2/3:电压通道
     *      peak:     峰值
     *
     *  @Return:
     *      true: success
     *     false: failed
     *
     *  @Note:
     *     在量程切换后，驱动的缓冲区包含了切换前后的AD采样值，应当忽略掉。
     */
    bool select_range_by_value( int chnl, double peak );
    const adc_range *adc_range_info(AD_CHNL_STYLE chnl);

    int getCycleCount() const;
    void setCycleCount(int value);

    int getNpPerCycle() const;
    int setNpPerCycle(int value);

    double getFreq() const;
    void setFreq(double value);

    bool start_adc();
    bool stop_adc();
    bool is_adc_started();

    double get_load_current();
    double get_output_vol();
    double get_load_vol();
    double get_pri_vol();

    bool isContinueSample();
    void setContinueSample(bool enabled = true);

    bool is_current_overflow(void);
    bool is_temperature_overflow();
    void clear_overflow_status(int flag);

    void startOverPowerProtect();
    void clearOverPowerStatus();
    void stopOverPowerProtect();

    void convToRealRange();

    void startRecord();
    void stopRecord();
    void recordWave(std::vector<double> &vol, std::vector<double> &cur);

private:
    adc_ctrl_private* _private;
    int np_per_cycle;
    int cycle_count;
    double freq;

    struct ThreadMutex m_mutex;

    friend class adc_ctrl_private;
};

#endif // ADC_CTRL_H
