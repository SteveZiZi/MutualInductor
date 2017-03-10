#ifndef AC_SOURCE_H
#define AC_SOURCE_H

#include "src/inc/tmutex.h"
#include "private_def.h"
#include <semaphore.h>

struct pid_info {
    bool reInit;
    double outputVol;
    double currentVol;
    float err;
    float err_pre;
    float errAcc;
    float kp,ki,kd;
};

struct DC_LEVEL_INFO;
class fpga_dev;
class raw_wave;

class ac_source
{
public:
    ac_source(fpga_dev* dev);
    ~ac_source();

    int startOutput();
    int stopOutput();
    bool outputStarted() { return outputStarted_; }

    int set_v(double v);
    double getOutputVoltage() {return outputVoltage_;}
    int dc_ac_f_set( double freq ); // in Hz ; 4Hz ~ 300Hz
    int dc_ac_p_set( double angle);

    int demagnetization();

    bool is_used_dc_12v();
    bool is_used_dc_200V() {return !is_used_dc_12v();}

    void usePidCtrl(bool use=true) {m_bUsePidCtrl = use;}
    bool pidCtrlUsed() {return m_bUsePidCtrl;}

    int get_indicate_sample_wave(raw_wave *wave_i_load,
                                 raw_wave *wave_u_src,
                                 raw_wave *wave_u_load,
                                 raw_wave *wave_u_pri);

private:


    bool dc_dc_start(void);
    bool dc_dc_stop(void);
    bool dc_ac_start(void);
    bool dc_ac_stop(void);
    bool dc_ac_enable(void);
    bool dc_ac_disable(void);
    
    int dc_dc_v200_set_percent( double pwm_percent );
    int dc_dc_v12_set_percent( double pwm_percent );
    int dc_ac_v_set_percent( double spwm_percent );
    int dc_ac_v_to_percent(double percent);
    int dc_dc_v200_to_percent( double percent );
    int dc_dc_v12_to_percent( double percent );


    static void pid_ctrl_thread(ac_source *p_this);

    int pid_ctrl();

private:
    double pwm_v200_percent;
    double pwm_v12_percent;
    double spwm_percent;

    bool outputStarted_;
    double outputVoltage_;

    bool is_dc12;

    fpga_dev* fpga;

    struct ThreadMutex m_mutex;
    bool killThread;
    struct pid_info pid;
    bool m_bUsePidCtrl;

    raw_wave *rawWave[MAX_CHNL_NUM];
    bool m_getRawWave;
    sem_t sem;

    friend class testAcSource;
    friend class CUserTest;
    friend class settingsWidget;
};

#endif // AC_SOURCE_H
