#ifndef CTPT_CTRL_P_H
#define CTPT_CTRL_P_H

#include <list>
#include <vector>
#include <complex>

#include <semaphore.h>
#include <pthread.h>

#include "ctpt_def.h"
#include "ctpt_api.hpp"
#include "src/fpga/fpga.hpp"
#include "src/fpga/raw_wave.hpp"
#include "data_type.hpp"
#include "status_mgr.h"
#include "splinefit.h"

#define angleToRadian(angle) ((angle)/180.0*M_PI)
#define radianToAngle(radian) ((radian)*180.0*M_1_PI) // M_1_PI = 1/M_PI

struct test_result;

namespace CTPT {

class ctpt_ctrl_p
{
public:
    ctpt_ctrl_p();
    virtual ~ctpt_ctrl_p();

public:
    static void main_thread( ctpt_ctrl_p *p_this );

    void run_test( void );

	//变比测试与参数计算
    int test_ratio(void);
    int find_ratio(void);
    int find_ratio(std::list<double> vol_primary, std::list<double> vol_secondary);
	void calculate_ratio_rated();	
    bool calculate_ratio_error();
    bool calculate_turns_ratio();
    bool calculate_phase_polar();

    /*
     * 励磁测量与计算
     */
     int test_excitation(void);
    int test_excitation_output(double freq, double max_current, std::vector<struct test_result> *list_result);
    int test_resistance(void);
    int	check_config(void);
    void set_all_channel_max_range(void);
    int set_output_freq_np(double freq);
    int output_transform_freq(double out_vol);
    int exciting_out_volte(double out_vol, bool wait_steaby=false);
    void copy_exciting( void );

    int to_indicate_cur(double cur);

    bool find_knee_point( struct auto_scan_result &r );
    bool find_knee_point( void );

    void calculate_resistance( void );
    bool calculate_5_percent_point(double resistance, double rated_current);
    bool calculate_10_percent_point(double resistance, double rated_current);
    void calculate_ct_ratio( void );
    bool calculate_inductance( void );
    bool calculate_inductance_unsaturated( void );
	bool calculate_remanence_factor(void);//计算剩磁系数
	void calculate_extend_param(void);	//计算不同类型CT的扩展参数
	void calculate_PX_ep(void); //计算PX的扩展参数
	void calculate_TPS_ep(void); //计算TPS的扩展参数
    bool calculate_Ktd(void); // 计算暂态面积系数
	
	//P、PR、计量级别
    bool calculate_alf_fs( void );
	bool find_U_I_ratio(double ratio, double& Iex);
	//计算二次极限感应电势
	bool calculate_sec_limit_emf(void);  
	//计算二次时间常数
	bool calculate_rated_secondary_loop_time_constant(void);
 
	//负载测试 
    int calculate_burden( void );
	

    // 电压源转电流源算法,　扫描输出，查找合适的电压
    bool auto_setup_output( double &vol, double freq, double vol_min, double current_max, double current_need );

    

	bool get_cur_ex_from_vol(  const double vol, double &cur, double &alpha);
    bool get_i_ex_from_vol( std::complex<double> &cur, const std::complex<double> &vol );

    double current_error( std::complex<double> z, // 负载
                          std::complex<double> r, // 内阻
                          double i_ct_abs,        //
                          double n_turns,
                          double n_rated );

    double current_error_vector( std::complex<double> z, // 负载
                                 std::complex<double> r, // 内阻
                                 std::complex<double> i_ct,        //
                                 double n_turns,
                                 double n_rated );
private:
    int pre_run_test();

public:
    status_mgr  *status_mgr_;
    struct auto_scan_result *exciting;
    struct auto_scan_result *ratio; 
    int    knee_index;
    double vol_knee;
    double cur_knee;
    struct ctpt_result result;

    //
    std::complex<double> zburden;
    std::complex<double> zresistance;

    // config
    struct ct_setting ct_setting;
    struct pt_setting pt_setting;

    sem_t sem;

    bool is_ct_mode;
    bool is_pt_mode;
    user_test_mode user_mode;
    volatile bool is_stopped;

    double user_vol;
    double user_cur;
    double user_freq;

    double Max_out_vol;
    double Max_out_current;

    double user_result_vol;
    double user_result_cur;

    int npoint_exciting;
    double *exciting_current;   // x coordinate
    double *exciting_voltage;   // y coordinate

    double output_vol_setting;

private:
    double test_frequency; 	//测试实际输出频率	
    std::vector<struct test_result> list_result; //励磁测量数据列表
    std::vector<double> fitVol;
    std::vector<double> fitCur;//拟合后的电压数据
	
	//饱和时电压电流波形数据
    std::vector<raw_wave> list_cur_secondary;
    std::vector<raw_wave> list_vol_secondary;
};

}


#endif // CTPT_CTRL_P_H
