#include "ac_source.h"

#include "fpga_reg.h"
#include "private_def.h"
#include "fpga.hpp"
#include "src/inc/auto_guard.h"
#include "src/inc/tmutex.h"
#include "raw_wave.hpp"

#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <iostream>
#include <pthread.h>


/*
****************************************************
** 模拟电源200V输出校准
**
**  每5%量程取一个校准区间
**  y = kx+b   x=希望输出的电压百分比  y=设定给FPGA的电压百分比
**
**  直流200V 输出占空比最好设置在10以上，低于10%输出波动较大
*****************************************************
*/
 struct dc_factor_t {
     float percent;    //  希望得到的百分比
     float k;          //  对应转换关系的k值
     float b;          //  对应转换关系的b值
 };

struct dc_factor_t dc_200_factor[] =
{
{1.624815, 0.000000, 1.000000},
{4.087316, 0.812183, -0.319646},
{5.894203, 1.106876, -1.524151},
{7.586824, 1.181600, -1.964590},
{9.246497, 1.205056, -2.142547},
{10.898241, 1.210842, -2.196047},
{12.543069, 1.215933, -2.251528},
{14.227729, 1.187183, -1.890919},
{15.918788, 1.182690, -1.826998},
{17.617928, 1.177066, -1.737466},
{19.352019, 1.153342, -1.319502},
{21.081782, 1.156227, -1.375334},
{22.830301, 1.143825, -1.113874},
{24.604262, 1.127421, -0.739350},
{26.393581, 1.117744, -0.501269},
{28.197127, 1.108927, -0.268544},
{30.019388, 1.097537, 0.052598},
{31.859415, 1.086940, 0.370719},
{33.725673, 1.071663, 0.857429},
{35.623991, 1.053564, 1.467831},
{37.513403, 1.058531, 1.290919},
{39.434782, 1.040919, 1.951599},
{41.358120, 1.039859, 1.993387},
{43.322304, 1.018235, 2.887731},
{45.297931, 1.012337, 3.143247},
{47.296192, 1.000871, 3.662630},
{49.323446, 0.986556, 4.339668},
{51.362538, 0.980829, 4.622139},
{53.428275, 0.968177, 5.271948},
{55.516447, 0.957776, 5.827698},
{57.620149, 0.950705, 6.220237},
{59.754257, 0.937160, 7.000719},
{61.907384, 0.928881, 7.495386},
{64.089614, 0.916494, 8.262264},
{66.288935, 0.909371, 8.718737},
{68.515201, 0.898365, 9.448323},
{70.765347, 0.888831, 10.101558},
{73.041903, 0.878520, 10.831211},
{75.345579, 0.868177, 11.586668},
{77.674570, 0.858741, 12.297659},
{80.029940, 0.849124, 13.044695},
{82.414380, 0.838771, 13.873174},
{84.836996, 0.825554, 14.962481},
{87.272227, 0.821277, 15.325317},
{89.739676, 0.810554, 16.261165},
{92.245231, 0.798226, 17.367419},
{94.806212, 0.780951, 18.961033},
{97.509538, 0.739829, 22.859581},
{100.10000, 0.804919, 16.512759}
};
#define DC_ORIGIN_200  203.287482          //match to hardware DC_200

/*
****************************************************
** 模拟电源12V输出校准
**
**  每5%量程取一个校准区间
**  y = kx+b   x=希望输出的电压百分比  y=设定给FPGA的电压百分比
**
**  直流12V 输出占空比最好设置在10以上，低于5%将按照0%输出。
**  直流12V 在占空比为0时，输出的电压恒定且不为0
*****************************************************
*/
struct dc_factor_t dc_12_factor[] =
{
{5.006358, 0.000000, 1.000000},
{6.316219, 1.526879, -6.644104},
{8.042680, 1.158439, -4.316954},
{9.865307, 1.097318, -3.825375},
{11.731561, 1.071666, -3.572310},
{13.620551, 1.058766, -3.420982},
{15.524152, 1.050641, -3.310303},
{17.436776, 1.045684, -3.233358},
{19.355367, 1.042431, -3.176638},
{21.277752, 1.040375, -3.136837},
{23.204063, 1.038254, -3.091705},
{25.131283, 1.037764, -3.080345},
{27.060908, 1.036471, -3.047848},
{28.991049, 1.036193, -3.040336},
{30.922866, 1.035295, -3.014284},
{32.853812, 1.035762, -3.028724},
{34.787486, 1.034300, -2.980707},
{36.722551, 1.033557, -2.954861},
{38.657654, 1.033536, -2.954092},
{40.592604, 1.033618, -2.957266},
{42.528657, 1.033029, -2.933357},
{44.465184, 1.032777, -2.922620},
{46.401092, 1.033107, -2.937278},
{48.339361, 1.031849, -2.878905},
{50.276810, 1.032285, -2.900013},
{52.214024, 1.032410, -2.906302},
{54.152069, 1.031968, -2.883192},
{56.086516, 1.033887, -2.987121},
{58.024447, 1.032029, -2.882885},
{59.963410, 1.031479, -2.851023},
{61.899134, 1.033205, -2.954495},
{63.838317, 1.031362, -2.840429},
{65.775069, 1.032657, -2.923061},
{67.710253, 1.033493, -2.978094},
{69.647756, 1.032257, -2.894367},
{71.585024, 1.032382, -2.903055},
{73.521480, 1.032815, -2.934051},
{75.455668, 1.034026, -3.023103},
{77.393442, 1.032112, -2.878724},
{79.326455, 1.034654, -3.075443},
{81.262078, 1.033259, -2.964772},
{83.427577, 0.923575, 5.948412},
{85.368956, 1.030196, -2.946715},
{87.314359, 1.028065, -2.764797},
{89.256547, 1.029766, -2.913393},
{91.197604, 1.030367, -2.966965},
{93.142073, 1.028558, -2.802053},
{95.084833, 1.029463, -2.886360},
{97.030586, 1.027880, -2.735760},
{98.971539, 1.030422, -2.982411},
{100.10000, 0.972327,  2.767302}
};
#define DC_ORIGIN_12        9.358          //match to hardware DC_12


struct dc_factor_t ac_factor[] =
{
{1.004623, 0.000000, 1.000000},
{2.988866, 1.007941, -0.012602},
{4.974738, 1.007114, -0.010129},
{6.961694, 1.006565, -0.007396},
{8.973076, 0.994341, 0.077701},
{10.960344, 1.006407, -0.030566},
{12.955891, 1.002231, 0.015200},
{14.941686, 1.007153, -0.048566},
{16.939696, 1.000996, 0.043428},
{18.920766, 1.009555, -0.101563},
{20.905444, 1.007720, -0.066835},
{22.901232, 1.002110, 0.050436},
{24.892109, 1.004582, -0.006175},
{26.876597, 1.007816, -0.086672},
{28.865207, 1.005728, -0.030543},
{30.853233, 1.006023, -0.039058},
{32.839053, 1.007141, -0.073544},
{34.829188, 1.004957, -0.001849},
{36.818597, 1.005323, -0.014594},
{38.808367, 1.005141, -0.007894},
{40.795060, 1.006698, -0.068320},
{42.787235, 1.003928, 0.044716},
{44.777427, 1.004929, 0.001886},
{46.768473, 1.004497, 0.021217},
{48.759508, 1.004503, 0.020945},
{50.748983, 1.005290, -0.017470},
{52.739240, 1.004895, 0.002589},
{54.725363, 1.006987, -0.107739},
{56.717481, 1.003956, 0.058127},
{58.691670, 1.013075, -0.459038},
{60.702220, 0.994753, 0.616298},
{62.688772, 1.006769, -0.113133},
{64.679880, 1.004466, 0.031262},
{66.655146, 1.012522, -0.489798},
{68.649870, 1.002645, 0.168576},
{70.647983, 1.000944, 0.285302},
{72.634802, 1.006635, -0.116704},
{74.650864, 0.992033, 0.943905},
{76.635228, 1.007880, -0.239105},
{78.623587, 1.005854, -0.083878},
{80.628311, 0.997644, 0.561683},
{82.592925, 1.018012, -1.080561},
{84.583174, 1.004900, 0.002397},
{86.557160, 1.013178, -0.697828},
{88.522679, 1.017543, -1.075625},
{90.505794, 1.008514, -0.276402},
{92.472093, 1.017139, -1.057009},
{94.450022, 1.011158, -0.503932},
{97.197283, 0.727998, 26.240576},
{99.153506, 1.022378, -2.372378},
{100.10000, 1.181343, -18.134333}
};

float real_percent(const dc_factor_t* factors, int size, double percent)
{
    if (percent < 0.01) {
        return 0.0;
    }
    if (percent >= 100.0) {
        return 100.0;
    }
    int n = 0;
    while(n < size) {
        if (percent < factors[n].percent)
            break;
        n++;
    }
    double realPercent = factors[n].k*percent+factors[n].b;
    if(realPercent > 100.0)
        realPercent = 100.0;

    return realPercent;
}

inline
float get_spwm_percent(double percent) {
    return real_percent(ac_factor, sizeof(ac_factor)/sizeof(dc_factor_t), percent);
}
inline
float get_pwm_percent(double percent) {
    return real_percent(dc_200_factor, sizeof(dc_200_factor)/sizeof(dc_factor_t), percent);
}
inline
float get_vpwm_percent(double percent) {
    return real_percent(dc_12_factor, sizeof(dc_12_factor)/sizeof(dc_factor_t), percent);
}

ac_source::ac_source(fpga_dev *dev)
    : pwm_v200_percent(0.0)
    , pwm_v12_percent(0.0)
    , spwm_percent(0.0)
    , outputStarted_(false)
    , outputVoltage_(0.0)
    , is_dc12(true)
    , fpga(dev)
    , killThread(false)
    , m_bUsePidCtrl(false)
    , m_getRawWave(false)
{
    sem_init(&this->sem, 0, 0);
//    pthread_t tid = 0;
//    pthread_create( &tid, NULL, (void* (*)(void*))pid_ctrl_thread, this);
//    std::cout << "ac_source tid ="<< tid << std::endl;
}

ac_source::~ac_source()
{
    m_bUsePidCtrl = false;
    killThread = true;
    usleep(100000);
    sem_destroy(&this->sem);
}

int ac_source::startOutput()
{
    if (outputStarted_) {
        return 0;
    }

    outputVoltage_ = 0.0;
    dc_ac_v_set_percent(0.0);
    dc_dc_v200_set_percent(0.0);
    dc_dc_v12_set_percent(0.0);

    dc_ac_p_set(90.0);

    dc_dc_start();
    dc_ac_enable();
    usleep(5000);
    dc_ac_start();

    outputStarted_ = true;

    return 0;
}

int ac_source::stopOutput()
{
    if (outputStarted_) {
        demagnetization();
    }
    dc_ac_stop();
    dc_dc_stop();
    dc_ac_disable();

    outputStarted_ = false;

    AUTO_GUARD(gd, struct ThreadMutex , m_mutex);
    m_bUsePidCtrl = false;
    return 0;
}

#define WAIT_STEADY(ms) \
    do {                                        \
        if (fpga && fpga->is_sample_started())  \
            fpga->wait_output_steaby();         \
        else                                    \
            usleep(ms*1000);                    \
    }while(0)
int ac_source::set_v(double vol)
{
    static const double DC200_100    = DC_ORIGIN_200;
    static const double DC200_100_90 = DC_ORIGIN_200*0.9;
    static const double DC200_10     = DC_ORIGIN_200*0.1;
    static const double DC200_10_90  = DC200_10*0.9;
    static const double DC200_10_30  = DC200_10*0.3;

    static const double DC12_100     = DC_ORIGIN_12;
    static const double DC12_100_90  = DC_ORIGIN_12*0.9;
    static const double DC12_10      = DC_ORIGIN_12*0.1;
    static const double DC12_10_0p9  = DC12_10*0.9;

    AUTO_GUARD(gd, struct ThreadMutex , m_mutex);

    double v = vol*M_SQRT2;     //有效值转换成峰值
    double pwm = 0.0, spwm = 0.0, vpwm = 0.0;
    bool changedDC = false;
    if (v>DC200_100_90) {         // vpwm:100  pwm:100  spwm:90~100
        vpwm = 100.0;
        pwm = 100.0;
        spwm = v/DC200_100*100.0;
        changedDC = is_dc12;
    }
    else if (v>DC200_10_90) {     // vpwm:100  pwm:10~100 spwm:90
        vpwm = 100.0;
        spwm = 90.0;
        pwm = v/DC200_100_90*100.0;
        changedDC = is_dc12;
    }
    else if (v>DC12_100_90) {     // vpwm:100  pwm:10 spwm:9/20*100~90
        vpwm = 100.0;
        pwm = 10.0;
        spwm = v/DC200_10*100.0;
        changedDC = is_dc12;
    }
    else if (v>DC200_10_30) {
        if (!is_dc12) {        // vpwm:6/10*100  pwm:10 spwm:6/20*100~9/20*100
            vpwm = DC200_10_30/DC12_100*100.0;
            pwm = 10.0;
            spwm = v/DC200_10*100.0;
        }
        else {                 // vpwm:6/9*100~100 pwm:0 spwm:90
            pwm = 0.0;
            vpwm = v/DC12_100_90*100.0;
            spwm = 90.0;
        }
    }
    else if (v > DC12_10_0p9) {   // vpwm:10~6/9*100 pwm:0 spwm:90
        pwm = 0.0;
        spwm = 90.0;
        vpwm = v/DC12_100_90*100.0;
        changedDC = !is_dc12;
    }
    else {                   // vpwm:10  pwm:0  spwm:0~90
        pwm = 0.0;
        vpwm = 10.0;
        spwm = v / DC12_10 * 100.0;
    }

    if (changedDC)
        is_dc12 = !is_dc12;
    dc_dc_v12_to_percent(get_vpwm_percent(vpwm));
    dc_dc_v200_to_percent(get_pwm_percent(pwm));
    if (is_dc12)
        spwm *= get_vpwm_percent(vpwm)/this->pwm_v12_percent;
    else
        spwm *= get_pwm_percent(pwm)/this->pwm_v200_percent;
    dc_ac_v_to_percent(get_spwm_percent(spwm));

    if (changedDC)
        WAIT_STEADY(500);

    outputVoltage_ = vol;
    printf("DC:%f  AC:%f\n", v, vol);
    printf("pwm:%f vpwm:%f spwm:%f\n", pwm, vpwm, spwm);
    printf("pwm:%f vpwm:%f spwm:%f\n", pwm_v200_percent, pwm_v12_percent, spwm_percent);

    //m_bUsePidCtrl = true;
    //pid.reInit = true;

    return 0;
}

int ac_source::dc_ac_f_set(double freq)
{
    uint32_t value = (uint32_t)((100000000ULL/1000ULL)/freq);
    return WRITE_THIS_REG(dc_dc_f_set, value); // 每两个点的时间间隔
}

int ac_source::dc_ac_p_set(double angle)
{
    uint32_t value = (uint32_t)(1000.0f*angle/360.0f);
    return WRITE_THIS_REG(dc_ac_p_set, value);
}


/*
*  @Func:  退磁
*  @Return:
*      0: success
*     -1: failed
*  @Note:
*      测试结束后，必须对CT/PT退磁操作
*/
int ac_source::demagnetization()
{
    usePidCtrl(false);
    if (this->outputVoltage_ < 0.001) {
        return 0;
    }

    printf("ac_source::demagnetization\n");
    if (this->outputVoltage_ > 50.0) {
        for (double v = this->outputVoltage_; v >= 30.0; v -= 10.0) {
            this->set_v(v);
            usleep(500000);
        }
    }
    if (this->outputVoltage_ > 15.0) {
        for (double v = this->outputVoltage_; v >= 10.0; v -= 5.0) {
            this->set_v(v);
            usleep(500000);
        }
    }
    if (this->outputVoltage_ > 1.0) {
        for (double v = this->outputVoltage_; v >= 0.5; v -= 2.0) {
            this->set_v(v);
            usleep(500000);
        }
    }
    for (double v = this->outputVoltage_; v >= 0.001; v -= 0.3) {
        this->set_v(v);
        usleep(100000);
    }
    this->set_v(0.0);
    usleep(10000);

    return 0;
}

bool ac_source::is_used_dc_12v()
{
    return is_dc12;
}

int ac_source::get_indicate_sample_wave(raw_wave *wave_i_load, raw_wave *wave_u_src, raw_wave *wave_u_load, raw_wave *wave_u_pri)
{
    this->rawWave[LOAD_CUR_CHNL] = wave_i_load;
    this->rawWave[SRC_VOL_CHNL]  = wave_u_src;
    this->rawWave[LOAD_VOL_CHNL] = wave_u_load;
    this->rawWave[PRI_VOL_CHNL]  = wave_u_pri;

    this->m_getRawWave = true;
    sem_wait(&this->sem);
    return 0;
}

bool ac_source::dc_dc_start()
{
    int bit = 1;
    uint32_t reg = READ_THIS_REG(dc_ac_start);
    WRITE_THIS_REG( dc_ac_start, reg|bit );
    return ( READ_THIS_REG(dc_ac_start) & bit ) ? true : false;
}

bool ac_source::dc_dc_stop()
{
    uint32_t bit = 1;
    uint32_t reg = READ_THIS_REG(dc_ac_start);
    WRITE_THIS_REG( dc_ac_start, reg&(~bit) );
    return ( READ_THIS_REG(dc_ac_start) & bit ) ? false : true;
}

bool ac_source::dc_ac_start()
{
    uint32_t bit = 4;
    uint32_t reg = READ_THIS_REG(dc_ac_start);
    WRITE_THIS_REG( dc_ac_start, reg|bit );
    return ( READ_THIS_REG(dc_ac_start) & bit ) ? true : false;
}

bool ac_source::dc_ac_stop()
{
    uint32_t bit = 4;
    uint32_t reg = READ_THIS_REG(dc_ac_start);
    WRITE_THIS_REG( dc_ac_start, reg&(~bit) );
    return ( READ_THIS_REG(dc_ac_start) & bit ) ? false : true;
}

bool ac_source::dc_ac_enable()
{
    uint32_t bit = 2;
    uint32_t  reg = READ_THIS_REG(dc_ac_start);
    WRITE_THIS_REG( dc_ac_start, reg|bit );
    return (READ_THIS_REG(dc_ac_start)&bit) ? true : false;
}

bool ac_source::dc_ac_disable(void)
{
    uint32_t bit = 2;
    uint32_t reg = READ_THIS_REG(dc_ac_start);
    WRITE_THIS_REG( dc_ac_start, reg&(~bit) );
    return ( READ_THIS_REG(dc_ac_start) & bit ) ? false : true;
}

int ac_source::dc_dc_v200_set_percent( double pwm_percent )
{
    uint32_t v = (uint32_t)(12.5 * pwm_percent);  // v = 1250 * percent / 100
    if (v > 1250)
        v = 1250;
    this->pwm_v200_percent = v*0.08;             // percent = v / 1250 * 100;
    return WRITE_THIS_REG( dc_dc_v200_set, v );
}

int ac_source::dc_dc_v12_set_percent( double pwm_percent )
{
    uint32_t v = (uint32_t)(12.5 * pwm_percent);  // v = 1250 * percent / 100
    if (v > 1250)
        v = 1250;
    this->pwm_v12_percent = v*0.08;               // percent = v / 1250 * 100;
    return WRITE_THIS_REG( dc_dc_v12_set, v );
}

int ac_source::dc_ac_v_set_percent(double spwm_percent)
{
    double dc_ac = spwm_percent*5.0/100.0;
    if ( dc_ac > 5.0 ) {
        dc_ac = 5.0;
        spwm_percent = 100.0;
    }
    this->spwm_percent = spwm_percent;
    return WRITE_THIS_REG( dc_ac_v_set, dc_ac );
}

int ac_source::dc_dc_v200_to_percent( double percent )
{
    if (this->pwm_v200_percent == percent) {
        return 0;
    }
    double step = (percent - this->pwm_v200_percent)/10;
    if (fabs(step) > 0.5) {
        for(int i= 0; i < 5; i++) {
            this->dc_dc_v200_set_percent(this->pwm_v200_percent+step*2);
            usleep(10000);
        }
    }
    this->dc_dc_v200_set_percent(percent);
    return 0;
}

int ac_source::dc_dc_v12_to_percent( double percent )
{
    if (this->pwm_v12_percent == percent) {
        return 0;
    }
    double step = (percent - this->pwm_v12_percent)/10;
    if (fabs(step) > 0.5) {
        for(int i= 0; i < 5; i++) {
            this->dc_dc_v12_set_percent(this->pwm_v12_percent+step*2);
            usleep(10000);
        }
    }
    this->dc_dc_v12_set_percent(percent);
    return 0;
}

void ac_source::pid_ctrl_thread(ac_source *p_this)
{
    printf("ac_source::pid_ctrl_thread\n");
    while(!p_this->killThread)
    {
        if (p_this->pidCtrlUsed())
        {
            AUTO_GUARD(gd, struct ThreadMutex, p_this->m_mutex);

            if (p_this->pid_ctrl() < 0)
                usleep(10000);
        }
        else {
            usleep(30000);
        }
    }
    printf("ac_source::pid_ctrl_thread exit\n");
}

int ac_source::pid_ctrl()
{
    if (pid.reInit) {
        pid.outputVol = outputVoltage_;
        pid.err = 0.0;
        pid.errAcc = 0.0;
        pid.err_pre = 0.0;
        pid.kp = 0.4;
        pid.ki = 0.05;
        pid.kd = 0.2;
        fpga->wait_output_steaby();
    }

    raw_wave srcWave;
    int ret = 0;
    if (!m_getRawWave) {
        ret = fpga->get_active_sample_wave(0, &srcWave, 0, 0);
        if (ret < 0)
            return -1;
    }
    else {
        raw_wave curWave, volWave, priWave;
        ret = fpga->get_active_sample_wave(&curWave, &srcWave, &volWave, &priWave);
        if (rawWave[LOAD_CUR_CHNL] != 0)
            *rawWave[LOAD_CUR_CHNL] = curWave;
        if (rawWave[SRC_VOL_CHNL] != 0)
            *rawWave[SRC_VOL_CHNL] = srcWave;
        if (rawWave[LOAD_VOL_CHNL] != 0)
            *rawWave[LOAD_VOL_CHNL] = volWave;
        if (rawWave[PRI_VOL_CHNL] != 0)
            *rawWave[PRI_VOL_CHNL] = priWave;
        m_getRawWave = false;
        sem_post(&this->sem);
        if (ret < 0)
            return -1;
    }

    srcWave.calculate_rms();
    pid.currentVol = srcWave.get_rms();

    pid.err = pid.outputVol-pid.currentVol;
    if (fabs(pid.err) < 0.005)
        return 0;

    double diff = pid.kp*pid.err+pid.ki*pid.errAcc+pid.kd*(pid.err-pid.err_pre);
    double percent = diff/pid.outputVol;
    if (percent>0.10)
        percent = 0.10;
    else if (percent<-0.10)
        percent = -0.10;

    percent = spwm_percent*(1.0+percent);
    if (percent < 0.001)
        percent = 0.0;
    dc_ac_v_set_percent(percent);
    pid.err_pre = pid.err;
    pid.errAcc += pid.err;

    return 0;
}

int ac_source::dc_ac_v_to_percent(double percent)
{
    if (this->spwm_percent == percent) {
        return 0;
    }
    double step = (percent - this->spwm_percent)/10;
    if (fabs(step) > 0.05) {
        for(int i= 0; i < 5; i++) {
            this->dc_ac_v_set_percent(this->spwm_percent+step*2);
            usleep(10000);
        }
    }
    this->dc_ac_v_set_percent(percent);
    return 0;
}

