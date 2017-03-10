#include "ctpt_ctrl_p.h"

#include <string.h>
#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>
#include <vector>
#include "src/fpga/raw_wave.hpp"
#include "lowess.h"
#include "splinefit.h"
#include "PolyFit.h"

using namespace CTPT;

//定义最小输出频率
#define MIN_FREQUENCY 1/*0.5*/

#define MAX_OUT_VOLTE 100
#define MIN_OUT_VOLTE 0.05

#ifndef NO_LOG
#define g_ctpt_log std::cout
#else
std::stringstream g_ctpt_log;
#endif

#if 0
#define _rms_()  get_base_rms()
#else
#define _rms_()  get_rms()
#endif
#define __safe_free(p) do{if (p) {free(p);p=0;}}while (0)


#define SAVE_ORIGIN_DATA   1

#define VOL_FILE "/system/ct/vol_wave_data.csv"
#define CUR_FILE "/system/ct/cur_wave_data.csv"
#define PRI_FILE "/system/ct/pri_wave_data.csv"
#define ALL_FILE "/system/ct/all_data.csv"


extern fpga_dev fpga;

struct test_result {
    double vol;
    double cur;
	double vol_base;
    double cur_base;
    double peak_vol;
    double peak_cur;
    double angle_vol;
    double angle_cur;

    double pri_vol;
	double pri_vol_base;
    double pri_vol_angle;

    test_result(const test_result& copyin) {
        this->vol = copyin.vol; this->cur = copyin.cur;
		this->vol_base = copyin.vol_base; this->cur_base = copyin.cur_base;
        this->peak_vol = copyin.peak_vol; this->peak_cur = copyin.peak_cur;
        this->angle_vol = copyin.angle_vol; this->angle_cur = copyin.angle_cur;
        this->pri_vol = copyin.pri_vol; this->pri_vol_angle = copyin.pri_vol_angle;
		this->pri_vol_base = copyin.pri_vol_base;
    }
    test_result() {
        this->vol = 0.0;
        this->cur = 0.0;
		this->vol_base = 0.0;
        this->cur_base = 0.0;
        this->peak_cur = 0.0;
        this->peak_vol = 0.0;
        this->angle_vol = 0;
        this->angle_cur = 0.0;
        this->pri_vol = 0.0;
        this->pri_vol_angle = 0.0;
		this->pri_vol_base = 0.0;
    }

    test_result &operator=(const test_result &rhs) {
        this->vol = rhs.vol; this->cur = rhs.cur;
		this->vol_base = rhs.vol_base; this->cur_base = rhs.cur_base;
        this->peak_vol = rhs.peak_vol; this->peak_cur = rhs.peak_cur;
        this->angle_vol = rhs.angle_vol; this->angle_cur = rhs.angle_cur;
        this->pri_vol = rhs.pri_vol; this->pri_vol_angle = rhs.pri_vol_angle;this->pri_vol_base = rhs.pri_vol_base; 
        return *this;
    }
    int operator==(const test_result &rhs) {
        if(this->vol != rhs.vol) return 0;
        if(this->cur != rhs.cur) return 0;
		if(this->vol_base != rhs.vol_base) return 0;
        if(this->cur_base != rhs.cur_base) return 0;
        if(this->peak_vol != rhs.peak_vol) return 0;
        if(this->peak_cur != rhs.peak_cur) return 0;
        if(this->angle_vol != rhs.angle_vol) return 0;
        if(this->angle_cur != rhs.angle_cur) return 0;
        if(this->pri_vol != rhs.pri_vol) return 0;
        if(this->pri_vol_angle != rhs.pri_vol_angle) return 0;
		if(this->pri_vol_base != rhs.pri_vol_base) return 0;
        return 1;
    }
};



ctpt_ctrl_p::ctpt_ctrl_p():
    exciting(0),
    ratio(0),
    knee_index(-1),
    vol_knee(0),
    cur_knee(0),
    is_ct_mode(false),
    is_pt_mode(false),
    user_mode(USER_TEST_RUN),
    is_stopped(true),
    Max_out_vol(MAX_OUT_VOLTE),
    Max_out_current(1),
    npoint_exciting(0),
    exciting_current(0),
    exciting_voltage(0)

{
    result.clear();
    output_vol_setting = 0.0;
    status_mgr_ = StatusMgrInst();

    pthread_t tid = 0;
    sem_init(&this->sem, 0, 0);
    pthread_create( &tid, NULL, (void* (*)(void*))main_thread, this);
    g_ctpt_log << "inited tid ="<< tid << std::endl;
    assert(tid);

    printf("CTPT: built at %s %s\n", __DATE__, __TIME__);
}


ctpt_ctrl_p::~ctpt_ctrl_p()
{
    sem_destroy(&this->sem);

    __safe_free(this->exciting_voltage     );
    __safe_free(this->exciting_current     );
}


void ctpt_ctrl_p::main_thread( ctpt_ctrl_p *p_this )
{
    printf("--- Start main thread ----\n");

    std::cout<< "main thread init" << std::endl;
    fpga.init( 4 );

    if (NULL == opendir("/system/ct"))
        mkdir("/system/ct", 0777);

    while(1) {
        sem_wait(&p_this->sem);

        if (p_this->user_mode == USER_TEST_RUN){
            printf("p_this->user_mode == USER_TEST_EXCITING\n" );
            p_this->run_test();
        }
        else if(p_this->user_mode == USER_TEST_SELFCHECK) {
            printf("p_this->user_mode == USER_TEST_SELFCHECK\n" );

        }
        else {
            printf("error p_this->user_mode is invalid\n");
        }
    }
}


// 根据电压向量查表得到历次电流,　励磁电流的角度已经被修正
bool ctpt_ctrl_p::get_i_ex_from_vol( std::complex<double> &cur, const std::complex<double> &vol )
{
    bool ok = true;
    int n;

    if ( !this->exciting )
    {
        return false;
    }
    n = this->exciting->wave_vol.size();
    double cur_prev = this->exciting->wave_cur[0].get_rms();
    std::complex<double> cur_prev_vector = this->exciting->wave_cur[0].get_w();
    double vol_ct = std::abs( vol );
    double vol_angle = std::arg( vol );
    for (int i=0; i<n; i++)
    {
        double vol_next;
        double cur_next;
        std::complex<double> cur_vector = this->exciting->wave_cur[i].get_w();
        vol_next = this->exciting->wave_vol[i].get_rms();
        cur_next = this->exciting->wave_cur[i].get_rms();

        if ( vol_next > vol_ct  )
        {
            double cur_abs = (cur_next + cur_prev)/2;
            double angle = ( std::arg(cur_vector) + std::arg(cur_prev_vector) )/2;
            cur = std::polar( cur_abs, angle+vol_angle );
            return true;
        }
        cur_prev = cur_next;
        cur_prev_vector = cur_vector;
    }

    return ok;
}


void ctpt_ctrl_p::run_test( void )
{
    bool rc = false;
    int ti = ct_setting.baseParam.testItem;


    result.clear();
    output_vol_setting = 0.0;


    printf("begin test! \n");
    if (pre_run_test() < 0) {
        goto cleanUp;
    }

    //测量直阻
    if((ti & ITEM_RESISTANCE) == ITEM_RESISTANCE)
    {
        status_mgr_->set_current_test_item(ITEM_RESISTANCE);
        printf("---- test_resistance -----\n");
        if (test_resistance() < 0)
            goto cleanUp;
    }
    //励磁特性测试
    if((ti & ITEM_EXCITATION) == ITEM_EXCITATION)
    {
        status_mgr_->set_current_test_item(ITEM_EXCITATION);
        printf("---- test_excitation -----\n");
        if (test_excitation() < 0)
            goto cleanUp;
    }
    //测量变比
    if((ti & ITEM_RATIO) == ITEM_RATIO)
    {
        status_mgr_->set_current_test_item(ITEM_RATIO);
        if(npoint_exciting == false)
            test_ratio();

        //变比参数计算
        calculate_ct_ratio();
    }
    //测量负载
    if((ti & ITEM_BURDEN )== ITEM_BURDEN)
    {
        status_mgr_->set_current_test_item(ITEM_BURDEN);
        if (calculate_burden() < 0)
            goto cleanUp;
    }

    rc =true;
cleanUp:
    status_mgr_->set_current_test_item(ITEM_DEMAGNETIOZATION);
    fpga.demagnetization();
    if (rc)
        status_mgr_->set_test_status(TEST_FINISHED);
    status_mgr_->set_testing(false);
}

/**********************************************
* @Desc:设定输出电压频率及其相应的每周期采样点
*		采样点数越高，计算RMS值误差越小，在保证计算
*		速度的情况下应尽量增加采样点数。
* @return:int
* @Author:zch
* @Date:2016.08.10
*
***********************************************/
int ctpt_ctrl_p::set_output_freq_np(double freq)
{
    int ret;
    int np = 512;
    fpga.dc_ac_f_set( freq );
    if(freq<10)
        np = 4096;
    else if(freq<20)
        np = 2048;
    else if(freq<30)
        np = 1024;
    ret = fpga.set_np_per_cycle( np );
    printf("--- Change the frequency to %f and sample points to %d ---\n",freq,np);
    return ret;
}
/**********************************************
* @Desc:当输出电压达到逆变最大值后，采用变频方法
*		进行等效电压输出相应频率，实际电压不变,
        为最大输出电压120V。
*		转换公式：U = f*U'/f'
* @return:int
* @Author:zch
* @Date:2016.08.10
*
***********************************************/
int ctpt_ctrl_p::output_transform_freq(double /*out_vol*/)
{
    return -1;
}
/**********************************************
* @Desc:励磁输出，当输出电压达到逆变最大值后，采用变频方法
*		进行等效电压输出。
* @return:int
* @Author:zch
* @Date:2016.08.10
*
***********************************************/
int ctpt_ctrl_p::exciting_out_volte(double out_vol, bool wait_steaby)
{
    int ret = 0;
    if (out_vol > Max_out_vol) {
        this->test_frequency = (Max_out_vol*ct_setting.baseParam.rated_frequency) / out_vol ;
        if(this->test_frequency < (double)(MIN_FREQUENCY)) {
            printf("!!!! Warning,Reach the Min frequency!!!!\n");
            return -1;
        }
        set_output_freq_np(this->test_frequency);
        ret = fpga.safe_output(Max_out_vol, wait_steaby);
    }
    else {
        ret = fpga.safe_output(out_vol, wait_steaby);
    }

    return ret;
}

/**********************************************
* @Desc:直流电阻测试，输出直流电压，测量绕组两端
*	   电压与电流，根据欧姆定律计算，取10组平均数
*		参考温度下直阻：Rct=(234.5+75)/(234.5+t),
*		75为参考温度
* @return:0正常返回，-1异常返回
* @Author:zch
* @Date:2016.06.27
*
***********************************************/
int ctpt_ctrl_p::test_resistance(void)
{
    int ret = 0;
    double currentTemp = ct_setting.baseParam.currentTemp + 234.5;
    double destTemp = ct_setting.baseParam.destTemp + 234.5;

    fpga.change_to_dc_source();
    fpga.start_output();
    fpga.adc_start_sample();

    raw_wave curWave, volWave;
    double output_val = 1.0;
    double step = 1.0;
    std::vector<double> rest;

    do {
        fpga.safe_output(output_val, true);
        ret = fpga.get_active_sample_wave(&curWave, 0, &volWave, 0);
        if (ret < 0)
            return -1;
        curWave.calculate_rms();
        volWave.calculate_rms();
        printf("vol:%f cur:%f res:%f----\n",
               volWave.get_rms(), curWave.get_rms(),
               volWave.get_rms()/curWave.get_rms());
        rest.push_back(volWave.get_rms() / curWave.get_rms());
        if (output_val>=4.9) {
            break;
        }
        output_val += step;
    }while(!is_stopped);

    fpga.change_to_ac_source();
    fpga.start_output();

    if (is_stopped) {
        status_mgr_->set_test_error_code(::get_error_code());
        status_mgr_->set_test_status(TEST_STOP_FOR_USER_CANCEL);
        return -1;
    }
    if (ret < 0) {
        status_mgr_->set_test_error_code(::get_error_code());
        status_mgr_->set_test_status(TEST_STOP_FOR_EXCEPTION);
        return -1;
    }

    double res = 0.0;
    for(unsigned int i = 0; i < rest.size(); i++) {
        res += rest[i];
    }
    res /= rest.size();

    //模拟数据
    result.widingRes.measRes = res;
    //换算到标准温度下的线圈直流电阻
    result.widingRes.refRes = res * destTemp / currentTemp;

    printf("Resistance test: %f, %f \n",result.widingRes.measRes,result.widingRes.refRes);
    return 0;
}

/**********************************************
* @Desc:计算匝数比
* @return:1正常返回，-1异常返回
* @Author:zch
* @Date:2016.07.23
*
***********************************************/
int ctpt_ctrl_p::find_ratio(std::list<double> vol_primary, std::list<double> vol_secondary)
{
    double ratio = 0;
    double vol_pri = 0;
    double vol_sec = 0;
	
    //计算变比，取中间2/3的多组值求平均值
    int n_size = vol_primary.size()/3;
    printf("The size of sample ratio is: %d \n", n_size);
    if(n_size<1)
    {
        printf("Too few sample!!!\n");
        return -1;
    }
    //先取出前1/3的点，丢弃
    for(int i=0; i<n_size; i++)
    {
        vol_primary.pop_front();
        vol_secondary.pop_front();
    }
    for(int i=0; i<n_size; i++)
    {
        vol_pri = vol_primary.front();
        vol_sec = vol_secondary.front();
        ratio += vol_sec / vol_pri;
        vol_primary.pop_front();
        vol_secondary.pop_front();
    }

    printf("The ratio is: %f \n", ratio / n_size);
    this->result.ratio.turns_ratio = ratio / n_size;

    return 1;
}

/**********************************************
* @Desc:计算匝数比
* @return:1正常返回，-1异常返回
* @Author:zch
* @Date:2016.07.23
*
***********************************************/
int ctpt_ctrl_p::find_ratio()
{
    int np = 0;
    std::list<double> vol_primary; // 初级线圈电压
    std::list<double> vol_secondary; // 次级线圈电压
    vol_primary.clear();
    vol_secondary.clear();

    if (list_result.size() != 0) {
        for(std::vector<struct test_result>::iterator it = list_result.begin(); it != list_result.end(); it++) {
            vol_primary.push_back(it->pri_vol);
            vol_secondary.push_back(it->vol);
            np++;
            if(np == this->knee_index)
                break;
        }
    }
    bool ret = find_ratio(vol_primary,vol_secondary);
	calculate_ratio_rated();
	printf(" Rated ratio is %f \r\n", this->result.ratio.ratio_rated);
	return ret;
}
//常用一次电流 
double commom_primary_current[]={5,10,15,20,25,30,40,50,60,75,100,150,200,250,300,400,500,600,750,800,1000,1200,1500,2000,2500,3000,4000,5000,6000,7500,10000,15000,20000,25000};
/**********************************************
* @Desc:计算额定匝数比
* @return:void
* @Author:zch
* @Date:2016.07.23
* 
***********************************************/
void ctpt_ctrl_p::calculate_ratio_rated()
{
	//计算额定匝数比，由用户设定的额定一次、二次电流计算
	this->result.ratio.ratio_rated = ct_setting.baseParam.primaryCurrent / ct_setting.baseParam.secondCurrent;
	//当用户没有设定额定一次电流时，由匝数比来推测额定变比
	if(this->result.ratio.ratio_rated == 0)
	{
		double nturn = this->result.ratio.ratio * ct_setting.baseParam.secondCurrent;
		if(nturn<commom_primary_current[0])
		{
			this->result.ratio.ratio_rated = commom_primary_current[0];
			return;
		}
		double pre = commom_primary_current[0];
		int n = sizeof(commom_primary_current)/ sizeof(double);
		//从常用一次电流表中查询最接近计算结果的额定一次电流
		for(int i=1; i<n; i++)
		{
			if(nturn<commom_primary_current[i])
			{
				if(nturn - pre < commom_primary_current[i] - nturn)
					this->result.ratio.ratio_rated = pre;
				else
					this->result.ratio.ratio_rated = commom_primary_current[i];
				return;
			}
			pre = commom_primary_current[i];
		}
	}
	
}
/**********************************************
* @Desc:测量变比，步进增加电压，测量绕组两端
*	   电压与电流，并计算变比
* @return:1正常返回，-1异常返回
* @Author:zch
* @Date:2016.06.27
*
***********************************************/
int ctpt_ctrl_p::test_ratio(void)
{
    printf( "radio running test\n");

    int ret = test_excitation_output(ct_setting.baseParam.rated_frequency, Max_out_current, &list_result);

    return ret;
}

/**********************************************
* @Desc:励磁输出，步进增加电压，测量绕组两端
*	   电压与电流，并计算拐点
* @return:1正常返回，-1异常返回， 2  找不到拐点
* @Author:zch
* @Date:2016.06.27
*
***********************************************/
int ctpt_ctrl_p::test_excitation_output(double /*freq*/, double max_current, std::vector<struct test_result> *list_result)
{
     int ret = -1;
     struct test_result result;
     double maxI = max_current;

     raw_wave exiting_cur;           // load current
     raw_wave pri_vol;               // primary voltage
     raw_wave sec_vol;               // load voltage

     double current_cur = 0.0;


#if SAVE_ORIGIN_DATA
    //将励磁数据记录到文件
    FILE *fp = fopen(VOL_FILE, "w");
    FILE *fp1 = fopen(CUR_FILE, "w");
    FILE *fp2 = fopen(PRI_FILE, "w");
	FILE *fpall = fopen(ALL_FILE, "w");
#endif
     if (maxI > 5.0) {
           maxI = 5.0;
     }
     printf("++maxI: %f ++\n", maxI );

     list_result->clear();
     npoint_exciting = 0;
     test_frequency = ct_setting.baseParam.rated_frequency;

    set_output_freq_np(ct_setting.baseParam.rated_frequency);

    current_cur = 0.0002;
     do {
         ret = this->to_indicate_cur(current_cur);
         if (ret < 0) {
             break;
         }
         printf("++get_active_sample_wave++\n");
         ret = fpga.get_active_sample_wave(&exiting_cur, 0, &sec_vol, &pri_vol);
         if (ret < 0) {
             break;
         }
         exiting_cur.calculate_all();
         sec_vol.calculate_all();
         pri_vol.calculate_all();
         result.vol = sec_vol._rms_() * ct_setting.baseParam.rated_frequency / test_frequency ; //U = f*U'/f'
         result.cur = exiting_cur._rms_();
		 result.vol_base = sec_vol.get_base_rms() * ct_setting.baseParam.rated_frequency / test_frequency ; //U = f*U'/f'
         result.cur_base = exiting_cur.get_base_rms();
         result.peak_vol = sec_vol.get_peek() * ct_setting.baseParam.rated_frequency / test_frequency;
         result.peak_cur = exiting_cur.get_peek();
         result.pri_vol = pri_vol._rms_() * ct_setting.baseParam.rated_frequency / test_frequency ; //U = f*U'/f'
		 result.pri_vol_base = pri_vol.get_base_rms() * ct_setting.baseParam.rated_frequency / test_frequency ; //U = f*U'/f'
         result.angle_vol = sec_vol.get_base_angle();
         result.angle_cur = exiting_cur.get_base_angle();
         result.pri_vol_angle = pri_vol.get_base_angle();

         printf("-------currnet:%f peak_cur:%f sec_vol:%f peak_vol:%f transform sec_vol:%f------\n",
                        result.cur, result.peak_cur, sec_vol._rms_(), sec_vol.get_peek(),result.vol);

#if SAVE_ORIGIN_DATA
        //写数据到文件
         if (fp){
            unsigned int np = sec_vol.get_np();
            fprintf(fp,"%f,%f,%f,%f,", result.vol,result.peak_vol, result.angle_vol, test_frequency );
            for(unsigned int i =0; i< np; i++)
                fprintf(fp,"%f,", sec_vol.get_writable_buffer()[i] );
            fprintf(fp,"\n");
         }
         if (fp1){
            unsigned int np1 = exiting_cur.get_np();
            fprintf(fp1,"%f,%f,%f,%f,", result.cur,result.peak_cur,result.angle_cur,test_frequency );
            for(unsigned int i1 =0; i1< np1; i1++)
                fprintf(fp1,"%f,", exiting_cur.get_writable_buffer()[i1] );
            fprintf(fp1,"\n");
             }
         if (fp2){
            unsigned int np2 = pri_vol.get_np();
            fprintf(fp2,"%f,%f,%f", result.pri_vol, result.pri_vol_angle,test_frequency );
            for(unsigned int i2 =0; i2< np2; i2++)
                fprintf(fp2,"%f,", pri_vol.get_writable_buffer()[i2] );
            fprintf(fp2,"\n");
         }
#endif
         list_result->push_back(result);
		 list_cur_secondary.push_back(exiting_cur);
		 list_vol_secondary.push_back(sec_vol);

         if (result.cur >= maxI || test_frequency < MIN_FREQUENCY) {
             break;
         }

         current_cur = result.cur*1.1;
         if (current_cur > maxI) {
             current_cur = maxI;
         }

     } while(!is_stopped); 

#if SAVE_ORIGIN_DATA

	if(fpall){
        unsigned int np3 = this->list_result.size();
		for(unsigned int i =0; i< np3; i++)
		{
            fprintf(fpall,"%f,%f,%f,%f,%f,%f,%f", this->list_result[i].vol,
                    this->list_result[i].cur,this->list_result[i].peak_vol,
                    this->list_result[i].peak_cur,this->list_result[i].angle_vol,
                    this->list_result[i].angle_cur,this->list_result[i].pri_vol_angle );
			fprintf(fpall,"\n");
		}
    }
    if(fp)
        fclose(fp);
    if(fp1)
        fclose(fp1);
    if(fp2)
        fclose(fp2);
	if(fpall)
        fclose(fpall);
#endif

     //异常停止
    if(ret < 0)  {
        printf("!!!exiting error!!!!\n");
        return -1;
    }
    if (is_stopped) {
        return -2;
    }
    
     return 0;
}

/**********************************************
* @Desc: 测试励磁曲线
* @return:1正常返回，-1异常返回
* @Author:zch
* @Date:2016.06.27
*
***********************************************/
int ctpt_ctrl_p::test_excitation(void)
{
    printf( "running exciting test\n");

    int ret = test_excitation_output(ct_setting.baseParam.rated_frequency, Max_out_current, &list_result);

    printf("list_result size is %d\n",list_result.size());
    for(std::vector<struct test_result>::iterator it = list_result.begin(); it != list_result.end(); it++)
        printf("%f,%f\n", it->vol, it->cur);

    switch(ret) {
    case 0:
        printf("exciting OK!\n");
        break;
    case -1:
        printf("stop unexpect!\n");
        status_mgr_->set_test_error_code(::get_error_code());
        status_mgr_->set_test_status(TEST_STOP_FOR_EXCEPTION);
        return -1;
    case -2:
        status_mgr_->set_test_error_code(::get_error_code());
        status_mgr_->set_test_status(TEST_STOP_FOR_USER_CANCEL);
        return -1;
        break;
    default:
        break;
    }
    //copy data to dst
    if (list_result.size() != 0) {
        int i = 0;
        int np = list_result.size();
        __safe_free(exciting_current );
        __safe_free(exciting_voltage );
        exciting_current   = (double*)malloc( np * sizeof(double) );
        if (!exciting_current) {
                printf("++p_this->exciting_current error++\n");
        }
        exciting_voltage   = (double*)malloc( np * sizeof(double) );
        if (!exciting_voltage) {
                printf("++p_this->exciting_voltage error++\n");
        }
        for(std::vector<struct test_result>::iterator it = list_result.begin(); it != list_result.end(); it++) {
            exciting_voltage[i] = it->vol;
            exciting_current[i] = it->cur;
            i++;
        }
        this->npoint_exciting = i;

        //进行参数计算
        find_knee_point();
        copy_exciting();
        calculate_10_percent_point(this->result.widingRes.measRes,this->ct_setting.baseParam.secondCurrent);
        calculate_5_percent_point(this->result.widingRes.measRes,this->ct_setting.baseParam.secondCurrent);
		calculate_sec_limit_emf();
		calculate_inductance_unsaturated(); 
		calculate_rated_secondary_loop_time_constant();
		calculate_remanence_factor();
		calculate_extend_param();			
    }
    return 1;
}
/**********************************************
* @Desc:复制励磁曲线到输出结果
* @Author:zch
* @Date:2016.06.27
*
***********************************************/
void ctpt_ctrl_p::copy_exciting( void )
{
    result.exciting_data.clear();
    for (uint32_t i=0; i<fitCur.size(); i++)
    {
        double vol = fitCur[i];
        double cur = fitVol[i];
        result.exciting_data.push_back(ui_wave_data(vol, cur));
    }

}

int ctpt_ctrl_p::to_indicate_cur(double cur)
{
    static double s_step = 0.1;
    static double s_cur0 = 0.0;

    int ret = 0;
    int tryCnt = 0;
    int errCnt = 0;
    bool overCur = false;  // trur: over indicator cur 50%
    bool finished = false;
    double cur_cur = 0.0;
    double cur_vol = 0.0;

    raw_wave curWave;
    raw_wave volWave;


    if (this->output_vol_setting < 0.001) {
        this->output_vol_setting = MIN_OUT_VOLTE;
        ret = exciting_out_volte(this->output_vol_setting);
        if (ret < 0)  return -1;
        ret = fpga.wait_output_steaby();
        if (ret < 0)  return -1;
        ret = fpga.get_active_sample_wave(&curWave, &volWave, 0, 0);
        if (ret < 0)  return -1;

        s_cur0 = fpga.get_load_current();
        if (s_cur0 > 0.0005)
            s_step = 0.05;
        else
            s_step = 0.1;
    }
tryAgain:
    tryCnt++;
    this->output_vol_setting  += s_step;
    ret = exciting_out_volte(this->output_vol_setting, overCur);
    if (ret < 0) {
        return -1;
    }
    ret = fpga.get_active_sample_wave(&curWave, &volWave, 0, 0);
    if (ret < 0) {
        return -1;
    }

    cur_cur = fpga.get_load_current();
    cur_vol = fpga.get_output_vol() * ct_setting.baseParam.rated_frequency / this->test_frequency;
    printf("+++++++ vol:%f  cur:%f  s_step:%f  indicate_cur:%f +++++++\n", cur_vol, cur_cur, s_step, cur);

    if (cur_cur > cur) {
        if ((cur_cur-cur)/cur > 0.1) {
            if (s_step > 0.1)
                s_step = 0.1;
        }
        finished = true;
        goto exitCode;
    }

    if (cur_cur < s_cur0) {
        if ((s_cur0-cur_cur)/s_cur0 > 0.1) {
            errCnt++;
            s_step = 0.2;
            if (errCnt>=5) {
                std::cout << "ctpt_ctrl_p::to_indicate_cur cur slowdown over five times" << std::endl;
                finished = true;
                goto exitCode;
            }
        }
    }
    else {
        // 0.9A ~ 5A
        if (cur_cur > 0.90) {
            s_step = cur_vol*0.001;
            if (s_step > 3.0)
                s_step = 3.0;
        }
        // 0.5 ~ 0.9A
        else if (cur_cur > 0.5) {
            s_step = (1-cur_cur)*cur_vol*0.005;
            if (s_step > 2.0)
                s_step = 2.0;
        }
        // 0.1A ~ 0.5A
        else if (cur_cur > 0.1) {
            s_step = cur_vol*0.002;
            if (s_step > 3.0)
                s_step = 3.0;
        }
        // 10mA ~ 100mA
        else if (cur_cur > 0.01) {
            s_step = cur_vol*0.01;
        }
        // 0 ~ 10mA
        else {
            s_step = cur_vol*0.02;
            if (s_step < 0.1)
                s_step = 0.1;
        }
        if (s_step > 5.0)
            s_step = 5.0;
        else if (s_step < 0.05)
            s_step = 0.05;
    }

    if (tryCnt < 300) {
        goto tryAgain;
    }

exitCode:
    s_cur0 = cur_cur;
    return (finished) ? 0 : -1;
}

/**********************************************
* @Desc:根据电压在励磁表查找励磁电流
* @Para: vol:电压值，cur:电流值，alpha:电流相角
* @Author:zch
* @Date:2016.06.27
*
***********************************************/
bool ctpt_ctrl_p::get_cur_ex_from_vol( const double vol, double &cur, double &alpha)
{
	int n;

	if ( list_result.size() <1 )
	{
		return false;
	}
	n = list_result.size();

	double vol_prev = list_result[0].vol_base;
	if(vol_prev > vol)
		return false;
	double cur_prev = list_result[0].cur_base;
	double alpha_prev = list_result[0].angle_cur;
	double alpah_vol_p = list_result[0].angle_vol;

	for (int i=1; i<n; i++)
	{
		double vol_next;
		double cur_next;
		double alpha_next;
		double alpha_vol_n;
		vol_next = list_result[i].vol_base;
		cur_next = list_result[i].cur_base;
		alpha_next = list_result[i].angle_cur;
		alpha_vol_n = list_result[i].angle_vol;
		if ( vol_next > vol  )
		{
			cur = (cur_next + cur_prev)/2;
			alpha = fabs((alpha_next+alpha_prev) - (alpha_vol_n + alpah_vol_p))/2;
			return true;
		}
		cur_prev = cur_next;
		vol_prev = vol_next;
		alpha_prev = alpha_next;
	}

	return false;
}


/**********************************************
* @Desc:查找拐点
*   	当电压升高10%时，电流升高50%（对于TPS为100%）
        则到达饱和拐点

* @Author:zch
* @Date:2016.06.27
*
***********************************************/
bool ctpt_ctrl_p::find_knee_point()
{
    int n = list_result.size();
    double incr = 1;

    knee_index = -1;

    if ( n < 1 )
    {
        assert( 0 && "never be n<1 ");
        return false;
    }

	//进行平滑处理
    std::vector<double> vx, vy, vyy;
    fitCur.clear();
    fitVol.clear();
	for (int i=0; i<n;i++)
	{
        vx.push_back(list_result[i].cur);
        vy.push_back(list_result[i].vol);
	}

    double f = 0.25;
    int nstep = 0;
    lowess(vx,vy,f,nstep,vyy);
    //int nstep = 5;//10阶
	//XT::PolyFit fit;
	//fit.polyfit(fitCur,vy,nstep,true);
	//fit.getFitedYs(fitVol);
	
    if (vyy.size()<1)
	{
		return false;
	}
	double pre_vol = vyy[0];
    double pre_cur = vx[0];
	//再次滤波
	for(int i=1; i<vyy.size(); i++)
	{
		if(pre_vol>vyy[i])
			continue;
		fitVol.push_back(vyy[i]);
		fitCur.push_back(vx[i]);
		pre_vol = vyy[i];
	}

	pre_vol = fitVol[0];
    pre_cur = fitCur[0];
    double deltaU = 0.0, deltaI = 0.0;
    double vol = 0.0, cur = 0.0;
    double k1 = 0.0, k2 = 0.0;
    std::vector<double> kneeks;
    for (unsigned int i = 1; i < fitCur.size(); i++)
    {
        vol = fitVol[i];
        cur= fitCur[i];
        deltaU = vol - pre_vol;
        deltaI = cur - pre_cur;
        k1 = deltaU *  pre_cur;
        k2 = deltaI * pre_vol;
        if(k1 != 0 && k2 !=0)
        {
            incr = k1 / k2;
            kneeks.push_back(incr);
        }
		else
            kneeks.push_back(1);
        pre_vol = vol;
        pre_cur = cur;
    }
    double maxvalue=0;
    int maxindex = 0;
    for(unsigned int i= 0; i<kneeks.size();i++)
    {
        if(maxvalue<kneeks[i])
        {
            maxvalue = kneeks[i];
            maxindex = i;
            printf("maxvlaue is %f and index is %d\r\n", maxvalue,maxindex);
        }

    }
    printf("maxvlaue is %f and index is %d\r\n", maxvalue,maxindex);
    for (unsigned int i= maxindex; i<kneeks.size();i++)
	{	
        if ( kneeks[i] <= 0.2)
        {
            // ok, found
            this->knee_index = i;

            this->vol_knee = (vy[i-1]+vy[i])/2;
            this->cur_knee = (fitCur[i-1]+fitCur[i])/2;


            //将拐点结果复制到输出
            this->result.excitation.Vkn = this->vol_knee;
            this->result.excitation.Ikn =  this->cur_knee;
            std::cout<< "found knee point @" << result.excitation.Vkn << "V " ;
            std::cout<< result.excitation.Ikn << "A" <<" knee_index: "<<knee_index<< std::endl;
            return true;
        }
	}
    return false;
}

/**********************************************
* @Desc:计算5%误差曲线点
* @Author:zch
* @Date:2016.06.27
*
***********************************************/
bool ctpt_ctrl_p::calculate_5_percent_point(double resistance, double rated_current)
{
    double Z2 = resistance;              //二次绕组阻抗，Z2 = Rct +jXct，Xct一般可忽略或取0.1或0.2

    int np = npoint_exciting;
    if(np <1)
        return false;

    //限定额定电流
    if(rated_current <1 || rated_current >5)
            rated_current = 5;

    double Kalf = 0;            //一次电流与额定一次电流比
    double ZLoad = 0;           //允许负载
    double Ie = 0;              //励磁电流
    double Es = 0;              //二次感应电动势

    this->result.error5_data.clear();
    for(int i =0; i<np; i++)
    {
        Ie = this->exciting_current[i];
        Es = this->exciting_voltage[i] - Ie*Z2;
        Kalf = 20*Ie / rated_current;
        ZLoad = Es/(19*Ie) - Z2;

        if(Kalf<1 || ZLoad <0)
            continue;
        this->result.error5_data.push_back(error_data(ZLoad, Kalf));
    }
    return true;
}
/**********************************************
* @Desc:计算10%误差曲线点
* @Author:zch
* @Date:2016.06.27
*
***********************************************/
bool ctpt_ctrl_p::calculate_10_percent_point(double resistance, double rated_current)
{
    double Z2 = 0;              //二次绕组阻抗，Z2 = Rct +jXct，Xct一般可忽略或取0.1或0.2
    if(resistance <=0)
         Z2 = 1;
    else
        Z2 = resistance;

    int np = npoint_exciting;
    if(np <1)
            return false;

    double Kalf = 0;            //一次电流与额定一次电流比
    double ZLoad = 0;           //允许负载
    double Ie = 0;              //励磁电流
    double Es = 0;              //二次感应电动势

    //限定额定电流
    if(rated_current <1 || rated_current >5)
        rated_current = 5;

    this->result.error10_data.clear();
    for(int i =0; i<np; i++ )
    {
        Ie = this->exciting_current[i];
        Es = this->exciting_voltage[i] - Ie*Z2;
        Kalf = 10*Ie / rated_current;
        ZLoad = Es/(9*Ie) - Z2;

        if(Kalf<1 || ZLoad <0)
            continue;

        this->result.error10_data.push_back(error_data(ZLoad, Kalf));
    }
    return true;
}

/**********************************************
* @Desc:计算二次极限感应电势和复合误差
*	    TPY级: Eal=Ktd×IN×K×(RB+RCT)
*		P 级（5P、10P、PR等）：Eal= IN×K×(RB+RCT)
*		Ktd：	暂态面积系数或者增大系数
        IN：		CT的二次额定电流（1A或者5A）
        K：		仪表保安系数（FS）或者准确限值系数（ALF）或者对称短路电流倍数（KSSC）
        RB：	负载的阻性分量
        RCT：	实际测量并换算到标准温度下的线圈直流电阻
        复合误差(或峰瞬误差) = Ial / (IN×K) （%） 
        Ial：极限电动势Eal对应的励磁电流
* @Author:zch
* @Date:2016.06.27
*
***********************************************/
bool ctpt_ctrl_p::calculate_sec_limit_emf(void)
{
    double In = ct_setting.baseParam.secondCurrent;
    double Rb = ct_setting.baseParam.ratedLoad / (In*In);
    double Rct = result.widingRes.refRes;

    printf("result.widingRes.refRes: %f\n",result.widingRes.refRes);

    double Eal = 0;
    double K = 0;
    double Ial = 0;
    //double alpha = 0;

	//进行曲线拟合
	xtalgorithm::spline fitline;

    printf("fitvol size is: %d, fitCur size is %d\r\n",fitVol.size(),fitCur.size());
	bool isFitOK = fitline.set_points(fitVol, fitCur);

    printf("fit ok!\r\n");
	switch (ct_setting.baseParam.level) {
	case CTC_MEASURE:
		K = ct_setting.extendParam.measure.fs;
		Eal = In * K * (Rb + Rct);
        printf("measure\r\n");
		if(isFitOK)
			Ial = fitline(Eal);
		if (In*K != 0 && isFitOK) {
			result.excitation.measure.composite_error = Ial /(In * K);
		}
		result.excitation.measure.Eal = Eal;
		break;
	case CTC_P:
		K = ct_setting.extendParam.p.alf;
		Eal = In * K * (Rb + Rct);
		if(isFitOK)
			Ial = fitline(Eal);
		if (In*K != 0 && isFitOK) {
			result.excitation.p.composite_error = Ial /(In * K);
		}
		result.excitation.p.Eal = Eal;
		break;
	case CTC_PR:
		K = ct_setting.extendParam.pr.alf;
		Eal = In * K * (Rb + Rct);
		if(isFitOK)
			Ial = fitline(Eal);
		if (In*K != 0 && isFitOK) {
			result.excitation.pr.composite_error = Ial /(In * K);
		}
		result.excitation.pr.Eal = Eal;
		break;
	case CTC_PX:
		K = ct_setting.extendParam.px.Kx;
		Eal = ct_setting.extendParam.px.Ek;
		if(isFitOK)
			Ial = fitline(Eal);
		if (In*K != 0 && isFitOK) {
			result.excitation.px.Ie = Ial;
			result.excitation.px.composite_error = Ial /(In * K);
		}
		break;
	case CTC_TPX:
		K = ct_setting.extendParam.tpx.kssc;
		Eal = ct_setting.extendParam.tpx.ktd * In * K * (Rb + Rct);
		result.excitation.tpx.Eal = Eal;
		if(isFitOK)
			Ial = fitline(Eal);
		if (In*K != 0 && isFitOK) { 
			result.excitation.tpx.peek_error = Ial /(In * K);
		}
		break;
	case CTC_TPY:
		K = ct_setting.extendParam.tpy.kssc;
		Eal = ct_setting.extendParam.tpy.ktd * In * K * (Rb + Rct);
		result.excitation.tpy.Eal = Eal;
		if(isFitOK)
			Ial = fitline(Eal);
		if (In*K != 0 && isFitOK) { 
			result.excitation.tpy.peek_error = Ial /(In * K);
		}
		break;
	case CTC_TPZ:
		K = ct_setting.extendParam.tpz.kssc;
		Eal = ct_setting.extendParam.tpz.ktd * In * K * (Rb + Rct);
		result.excitation.tpz.Eal = Eal;
		if(isFitOK)
			Ial = fitline(Eal);
		if (In*K != 0 && isFitOK) { 
			result.excitation.tpz.peek_error = Ial /(In * K);
		}
		break;
    case CTC_TPS:
		if(isFitOK)
		{
			Eal = ct_setting.extendParam.tps.Ual;
			Ial = fitline(Eal);
			result.excitation.tps.Ial = Ial;
		} 
        break;
    default:
        break;
    }
printf("Ial is %f\r\n",Eal);
    printf("calc Eal OK \r\n");
    return true;
}
/**********************************************
* @Desc:计算不饱和电感
*	    计算的是磁化曲线上20％和90％拐点间的线性范围内的平均电感，
        Lu = (Lm20% + ... +Lm90%)/n, 其中Lm = Ψm / Im = U / (4.4429 * f * im)
        im为U对应的励磁曲线上电流点的峰值
*
* @Author:zch
* @Date:2016.06.27
*
***********************************************/
bool ctpt_ctrl_p::calculate_inductance_unsaturated( void )
{
    bool ok = true;
    int start_index = this->knee_index * 0.2;
    int stop_index = this->knee_index * 0.9;
    double sum = 0.0;
    int count = 0;

    for (int i=start_index; i<=stop_index; i++, count++)
    {
        double vol = this->exciting_voltage[start_index];
        double cur = this->exciting_current[start_index];
        double Lm = vol / (4.4429 * ct_setting.baseParam.rated_frequency * cur);
        sum += Lm;
    }
    if ( count == 0 )
    {
        this->result.excitation.Lu = -1;
        return false;
    }
    this->result.excitation.Lu = sum / count;
    return ok;
}
/**********************************************
* @Desc:计算二次时间常数
*	    电流互感器的二次回路时间常数的值可以从 CT 的电感和电阻和二次回路的电感和电阻之和获得。
        Ts = Ls/Rs;
        RS = Rb + Rct75°C	Rb = Rwire + Rrelay	LS = Lu + Lb
        LS：	磁化电抗和漏抗之和，计算中忽略
        Lu：	CT的不饱和电抗
        Lb：	测量负载的电抗，计算中忽略
        RS：	二次回路电阻
        RCT：	CT电阻在75°C时
        Rb：	测量负载的阻性部分,取额定负载
* @Author:zch
* @Date:2016.08.20
*
***********************************************/
bool ctpt_ctrl_p::calculate_rated_secondary_loop_time_constant()
{
	double Isn = ct_setting.baseParam.secondCurrent; 
    double Ts = -1;
    double Rct = this->result.widingRes.refRes;
    double Lu = this->result.excitation.Lu;
    double Rb = ct_setting.baseParam.ratedLoad / (Isn*Isn);
    double Rs = Rb + Rct;
    if(Rs > 0)
    {
        Ts = Lu / Rs;
        result.excitation.Ts = Ts;
        return true;
    }
    return false;
}

/**********************************************
* @Desc: 计算剩磁系数
* @Author:zch
* @Date:2016.11.5
***********************************************/
bool ctpt_ctrl_p::calculate_remanence_factor()
{
    printf("calculate_remanence_factor\r\n");
    double Rct = this->result.widingRes.refRes;
	std::vector<double> flux;
    printf("size of list vol and cur is %d %d\r\n",list_vol_secondary.size(),list_cur_secondary.size());
	std::vector<double> U = list_vol_secondary[knee_index].get_writable_buffer();
	std::vector<double> Iex = list_cur_secondary[knee_index].get_writable_buffer();
    printf("U size and Iex sise is %d %d\r\n", U.size(),Iex.size());
    //assert(U.size()==Iex.size());
    //if(U.size()<256 )
    //{
    //    printf("return false\r\n" );
    //	return false;
    //
    int rawsize = list_vol_secondary[knee_index].get_np_per_cycle();
    printf("knee index is %d\r\n", rawsize );
    unsigned int npoints = rawsize;//计算一个周期内的
	double integral = 0.0;
    printf("npoints is %d",npoints);
	//计算电压积分
    for(unsigned int i=0; i<npoints; i++)
	{
		integral += U[i]-Rct*Iex[i];
		flux.push_back(integral);
	}
	
	//找出饱和磁通Ψs和剩磁通Ψr
	double phys = flux[0];
	int phyr_index = 0;
	int phyr1_index = 0;
	int phyr2_index = 0;
	int scope = 30;   //寻找过零点范围
	int scope_index =0;
	double minIex = 1;
	bool begin_count = false;
	double Iex_pre = Iex[0];
    for(unsigned int i=1; i<npoints; i++)
	{
		if(phys<flux[i])
			phys = flux[i];//找出饱和磁通Ψs
			
		if(Iex_pre*Iex[i]<0 && scope_index < scope)
		{
			if(fabs(minIex) > fabs(Iex_pre*Iex[i]))
			{
				minIex = Iex_pre*Iex[i];
				phyr_index = i;
			}
			begin_count = true;
		}
		if(begin_count)
		{
			scope_index ++;
			if(scope_index>scope) //查找结束
			{
				begin_count = false;
				scope_index = 0;
				if(phyr1_index == 0)
				{
					phyr1_index = phyr_index;
				}
				else{
					phyr2_index = phyr_index;
				}
			}
		}
		Iex_pre = Iex_pre*Iex[i];		
	}
    printf("remanence ok\r\n");
	//计算剩磁系数
	double Kr = fabs((flux[phyr1_index]+flux[phyr1_index-1])/2 - (flux[phyr2_index]+flux[phyr2_index-1])/2) /  phys;
    this->result.excitation.Kr = Kr*100;
	printf(" The remanence factor is %f \r\n", result.excitation.Kr);
	return true;
}

/**********************************************
* @Desc: 计算扩展参数
* @Author:zch
* @Date:2016.11.5
***********************************************/
void ctpt_ctrl_p::calculate_extend_param()
{
	calculate_alf_fs();	
	calculate_PX_ep();
	calculate_TPS_ep();
}
/**********************************************
* @Desc: 计算PX扩展参数
* Ek = Kx*(Rct+Rb)*Isn
* @Author:zch
* @Date:2016.11.5
***********************************************/
void ctpt_ctrl_p::calculate_PX_ep()
{
	if(ct_setting.baseParam.level != CTC_PX)
		return; 
	     
    double Isn = ct_setting.baseParam.secondCurrent;
    double Rct = this->result.widingRes.refRes;
	double Rb = ct_setting.baseParam.ratedLoad / (Isn*Isn);	
	
	result.excitation.px.Ek = ct_setting.extendParam.px.Ek;
	result.excitation.px.Kx = result.excitation.px.Ek / ((Rct+Rb)*Isn);	
	//Ie, composite_error 在 calculate_sec_limit_emf() 中已计算 
}

/**********************************************
* @Desc: 计TPS扩展参数 
* @Author:zch
* @Date:2016.11.5
***********************************************/
void ctpt_ctrl_p::calculate_TPS_ep()
{
	if(ct_setting.baseParam.level != CTC_TPS)
		return; 
	     	
	result.excitation.tps.Ual = ct_setting.extendParam.tps.Ual;
	//result.excitation.tps.Ial 在 calculate_sec_limit_emf() 中已计算 
}


/**********************************************
* @Desc: 计算暂态面积系数
* @Author:zch
* @Date:2016.11.20
* 
***********************************************/
double calculate_C_O_Ktd(double w, double Ts, double Tp, double t1, double t1al)
{
	double Ktd = (w*Ts*Tp/(Tp-Ts))*(exp((-t1al)/Tp) -exp((-t1al)/Ts)) + 1;
	return Ktd;
}
double calculate_C_O_C_O_Ktd(double w, double Ts, double Tp, double t1, double t1al, double tfr, double t2, double t2al)
{
	double Ktd = ((w*Ts*Tp/(Tp-Ts))*(exp((-t1)/Tp) -exp((-t1)/Ts)) - sin(w*t1))*exp(-(tfr+t2al)/Ts)
				 + (w*Ts*Tp/(Tp-Ts))*(exp((-t2al)/Tp) -exp((-t2al)/Ts)) + 1;
	return Ktd; 
}
bool ctpt_ctrl_p::calculate_Ktd()
{
	double w = 314.159265358;
	double Ts = 0;
	double Tp = 0;
	double t1 = 0;
	double t1al = 0;
	double tfr = 0;
	double t2 = 0;
	double t2al = 0; 
	
	switch(ct_setting.baseParam.level) {
	case CTC_TPX:
		Ts = ct_setting.extendParam.tpx.Ts;
		Tp = ct_setting.extendParam.tpx.Tp;
		t1 = ct_setting.extendParam.tpx.t1;
		t1al = ct_setting.extendParam.tpx.tal1;
        if(ct_setting.extendParam.tpx.coc == 0)
		{ 
			result.excitation.tpx.ktd = calculate_C_O_Ktd(w,Ts,Tp,t1,t1al);
		}
        else if(ct_setting.extendParam.tpx.coc == 1)
		{
			t2 = ct_setting.extendParam.tpx.t2;
			tfr = ct_setting.extendParam.tpx.tfr;
			t2al = ct_setting.extendParam.tpx.tal2;
			result.excitation.tpx.ktd = calculate_C_O_C_O_Ktd(w, Ts, Tp, t1, t1al, tfr, t2, t2al);
		} 
		break;
	case CTC_TPY:
		Ts = ct_setting.extendParam.tpy.Ts;
		Tp = ct_setting.extendParam.tpy.Tp;
		t1 = ct_setting.extendParam.tpy.t1;
		t1al = ct_setting.extendParam.tpy.tal1;
        if(ct_setting.extendParam.tpy.coc == 0)
		{ 
			result.excitation.tpy.ktd = calculate_C_O_Ktd(w,Ts,Tp,t1,t1al);
		}
        else if(ct_setting.extendParam.tpy.coc == 1)
		{
			t2 = ct_setting.extendParam.tpy.t2;
			tfr = ct_setting.extendParam.tpy.tfr;
			t2al = ct_setting.extendParam.tpy.tal2;
			result.excitation.tpy.ktd = calculate_C_O_C_O_Ktd(w, Ts, Tp, t1, t1al, tfr, t2, t2al);
		} 
		break; 
	case CTC_TPZ: 
        {
			Ts = ct_setting.extendParam.tpz.Ts;
			Tp = ct_setting.extendParam.tpz.Tp;
			t1 = ct_setting.extendParam.tpy.t1;
			t1al = ct_setting.extendParam.tpy.tal1;
			result.excitation.tpz.ktd = calculate_C_O_Ktd(w, Ts, Tp, t1, t1al);
        }
		break;
    default:
        break;  
    }
	return true;
}
/**********************************************
* @Desc: 设置所有通道为最大量程
* @Author:zch
* @Date:2016.08.20
*
***********************************************/
void ctpt_ctrl_p::set_all_channel_max_range()
{
    fpga.set_max_range();
}

/**********************************************
* @Desc: 变比参数计算
* @Author:zch
* @Date:2016.08.20
*
***********************************************/
void ctpt_ctrl_p::calculate_ct_ratio(  )
{
    find_ratio();
    calculate_ratio_error();
    calculate_turns_ratio();
    calculate_phase_polar();
}

/**********************************************
* @Desc: 计算比值差和角差
* @Author:zch
* @Date:2016.08.20
*
***********************************************/
bool ctpt_ctrl_p::calculate_ratio_error()
{
    // 比值差
    double rated_burden_100 = ct_setting.baseParam.ratedLoad; 		//额定输出功率
    double rated_power_factor = ct_setting.baseParam.powerFactor;	//额定功率因数
    double Isn = ct_setting.baseParam.secondCurrent;
    double Rct = this->result.widingRes.refRes;
    double Nturns = this->result.ratio.turns_ratio;			//匝比
    double Nrated  = this->result.ratio.ratio_rated;	//额定变比

	//没有励磁数据无法计算角差比差
	if(list_result.size()<1)
	{
		return false;
	}

    // 只计算100%，50%，25%和0%额定输出功率
    result.ratio_error_data.resize(4);
    result.phase_error_data.resize(4);

    result.ratio_error_data[0].VA = result.phase_error_data[0].VA = rated_burden_100;           //100%额定输出功率
    result.ratio_error_data[1].VA = result.phase_error_data[1].VA = rated_burden_100*0.5;       //50%额定输出功率
    result.ratio_error_data[2].VA = result.phase_error_data[2].VA = rated_burden_100*0.25;      //25%额定输出功率
    result.ratio_error_data[3].VA = result.phase_error_data[3].VA = rated_burden_100*0.0;       //0%额定输出功率

    result.ratio_error_data[0].factor = result.phase_error_data[0].factor = rated_power_factor; //100%额定输出功率
    result.ratio_error_data[1].factor = result.phase_error_data[1].factor = rated_power_factor; //50%额定输出功率
    result.ratio_error_data[2].factor = result.phase_error_data[2].factor = rated_power_factor; //25%额定输出功率
    result.ratio_error_data[3].factor = result.phase_error_data[3].factor = 1;                  //0%额定输出功率


	//Ict取1%，5%，20%，100%，120% 额定二次电流
	double Ict_factor[5] = {0.01, 0.05, 0.2, 1, 1.2};
	double error_current[5];
	double error_phase[5];
	
	double Zb,Rb,Xb;
	double cos_phy,sin_phy; 
	 
    for(int i=0;i<4;i++)
    {
		//step1: 负载阻抗的实部和虚部计算如下: RB=ZB×cosφ；XB=ZB×sinφ, ZB = Va / Isn^2
        cos_phy = result.ratio_error_data[i].factor;
		sin_phy = sqrt(1-cos_phy*cos_phy);		
        Zb = result.ratio_error_data[i].VA / (Isn * Isn);
		Rb = Zb * cos_phy;
		Xb = Zb * sin_phy; 
		//step2: 总负载阻抗的计算 Rload＝RCT＋RB，Xload＝XB，Zload^2＝Rload^2＋Xload^2, gama = artan(Xload/Rload)
		double Rload = Rct + Rb;
		double Zload = sqrt(Rload*Rload + Xb*Xb);
		double gama = atan( Xb / Rload);  //单位，弧度
		//step3: 给定总的二次电流Ict,计算二次励磁电压VC=ICT×Zload。 Ict取1%，5%，20%，100%，120% 额定二次电流
		for(int j=0;j<5;j++)
		{
			printf("j: %d\n",j);
			double Ict = Isn * Ict_factor[j];
			double Vc  = Ict * Zload;
			double alpha, Iex;
            //step4: 在励磁表格上查找对应于这个电压和相位α的励磁电流Iex<>VC；α=∠Iex,VC
			get_cur_ex_from_vol(Vc, Iex, alpha ); 
            //step5: 计算二次负载电流和励磁电流间的相位 β=α-γ
			double beta = alpha - gama;
            //step6: 计算总二次电流： Ist'  = Ict' + Iex',  Ist = sqrt(( Ict + Iex*cosβ)^2 + (Iex*sinβ)^2)
			double Ist = sqrt((Ict+Iex*cos(beta))*(Ict+Iex*cos(beta))+(Iex*sin(beta))*(Iex*sin(beta)));
            //step7:计算一次电流 Ip = Ist*Nturns;
			double Ip = Ist * Nturns;
            //step8: 计算比差值 f = (Ict*Nrated)/Ip - 1
			error_current[j] = (Ict*Nrated)/Ip - 1;
			
            //step9: 计算相差 delta = arctan(Iex*sinβ / (Ict+Iex*cosβ))
			double delta = atan(Iex*sin(beta) / (Ict+Iex*cos(beta)));
            error_phase[j] = radianToAngle(delta)*60;  //角速度转换成分
			std::cout<<"error_current:"<<error_current[j]<<" error_phase:"<<error_phase[j]<<std::endl;
        }
        result.ratio_error_data[i].percent1   = error_current[0];      // 1%   额定二次电流
        result.ratio_error_data[i].percent5   = error_current[1];      // 5%   额定二次电流
        result.ratio_error_data[i].percent20  = error_current[2];      // 20%  额定二次电流
        result.ratio_error_data[i].percent100 = error_current[3];      // 100% 额定二次电流
        result.ratio_error_data[i].percent120 = error_current[4];      // 120% 额定二次电流

        result.phase_error_data[i].percent1   = error_phase[0];      // 1%   额定二次电流
        result.phase_error_data[i].percent5   = error_phase[1];      // 5%   额定二次电流
        result.phase_error_data[i].percent20  = error_phase[2];      // 20%  额定二次电流
        result.phase_error_data[i].percent100 = error_phase[3];      // 100% 额定二次电流
        result.phase_error_data[i].percent120 = error_phase[4];      // 120% 额定二次电流
	}
	 
	return true;
}
/**********************************************
* @Desc: 计算变比和匝比误差
* @Author:zch
* @Date:2016.08.20
*	计算变比：
	1、	求出100%额定电流的比差f。
	2、	求额定变比Kn = Ipn/Isn;
	3、	根据 f=(Kn*Is – Ip)/Ip，可求得Kturns = Ip/Is = Kn(f+1)
***********************************************/
bool ctpt_ctrl_p::calculate_turns_ratio()
{
	double f = this->result.ratio_error_data[3].percent100;  //100%额定功率，100% 额定二次电流
	this->result.ratio.ratio = this->result.ratio.ratio_rated * (f+1);
	//计算匝比误差： ratio_error = (Nmeassured - Nrated)/Nrated
	this->result.ratio.turns_ratio_error = (this->result.ratio.turns_ratio - this->result.ratio.ratio_rated)/this->result.ratio.ratio_rated;
	
	// 比值差
	this->result.ratio.ratio_error = f;   
	// 相位差	
	this->result.ratio.phase_error = this->result.phase_error_data[3].percent100;   
	
	return true;
}
/**********************************************
* @Desc: 计算极性
* @Author:zch
* @Date:2016.08.20
*
***********************************************/
bool ctpt_ctrl_p::calculate_phase_polar()
{
	int np = this->knee_index;
	if(np < 1)
		return false;
		
	if (list_result.size() != 0) { 
        double angle_sec = radianToAngle(list_result[np/2].angle_vol);
        double angle_pri = radianToAngle(list_result[np/2].pri_vol_angle);
		
		//利用相差计算极性。
        double delta = fabs(angle_sec - angle_pri);
		if(delta >150 && delta <181)
		{
            result.ratio.phase_polar = false;
			printf("phase polar is negative\n");
		}
		else if(delta >=0 && delta <30){
            result.ratio.phase_polar = true;
			printf("phase polar is positive\n");
		}
		return true;
    } 
	return false;
}






static double resistance( const raw_wave &vol, const raw_wave &cur )
{
    // double rms_vol;
    double rms_cur;
    int np;
    double active_power;
    double r;

    // rms_vol = vol.get_rms();
    rms_cur = cur.get_rms();

    np = vol.get_np_ncycled();
    active_power =  vol.get_multiply( cur, np ) / np;

    // P = U^2 / R
    // P = I^2 * R
    r = active_power / ( rms_cur * rms_cur );

    return r;
}

void ctpt_ctrl_p::calculate_resistance( void )
{
    double sum = 0;
    int i;
    int n;

    n = this->knee_index;
    for (i = 0; i<n; i++)
    {
        double r;
        raw_wave &vol = exciting->wave_vol[i];
        raw_wave &cur = exciting->wave_cur[i];
        r = resistance( vol, cur );
        sum += r;
    }
    double res;
    res = sum / n;
    result.widingRes.measRes = res;
}


double ctpt_ctrl_p::current_error( std::complex<double> z, // 负载
                                         std::complex<double> r, // 内阻
                                         double i_ct_abs,        //
                                         double n_turns,
                                         double n_rated )
{
    // 负载主抗的实部和虚部
    // double rb = z.real();
    // double ib = z.imag();

    // 总负载阻抗的计算
    std::complex<double> zload = z + r;
    // double  zload_abs = std::abs( zload );
    double gama = std::arg(zload);

    // 给定总的二次电流: 参数传入

    // 计算二次励磁电压
    // double vc = i_ct_abs * zload_abs;

    // 在励磁表格上查找对应于这个vc电压和相位v的励磁电流
    double i_ex_abs = 0;
    double v = 0;
    // std::complex<double> i_ex = std::polar( i_ex_abs, 0.0 );

    // if ( !get_i_ex_from_vol( vc, &i_ex_abs, &v ) )
    // {
    //     return ;
    // }

    // 计算二次负载电流和励磁电流间的相位
    double beta = v - gama;

    // 计算总二次电流
    double i_st_abs;
    double t1, t2;
    t1 = i_ct_abs + i_ex_abs * cos( beta );
    t2 = i_ex_abs * sin( beta );
    i_st_abs = sqrt( (t1*t1) + (t2*t2) );

    // 计算一次电流
    double i_p_abs = i_st_abs * n_turns;

    // 计算比差
    double err_ratio = ( i_ct_abs - i_p_abs/n_rated ) / ( i_p_abs/n_rated );

    // 计算Ist和Ict间相位差
    // double theta = atan( i_ex_abs * sin(beta) / ( i_ct_abs + i_ex_abs*cos(beta) ) );
    return err_ratio;
}


double ctpt_ctrl_p::current_error_vector( std::complex<double> z, // 负载
                                                std::complex<double> r, // 内阻
                                                std::complex<double> i_ct,        //
                                                double n_turns,
                                                double n_rated )
{
    // 负载主抗的实部和虚部
    // double rb = z.real();
    // double ib = z.imag();
    double i_ct_abs = std::abs(i_ct);

    // 总负载阻抗的计算
    std::complex<double> zload = z + r;
    // double  zload_abs = std::abs( zload );
    // double gama = std::arg(zload);

    // 给定总的二次电流: 参数传入

    // 计算二次励磁电压
    std::complex<double> vc = i_ct * zload;

    // 在励磁表格上查找对应于这个vc电压和相位的励磁电流 i_ex
    std::complex<double> i_ex;

    if ( !get_i_ex_from_vol( i_ex, vc ) )
    {
        g_ctpt_log << "ERROR: exciting not ready.\n";
        return 0;
    }

    // 计算二次负载电流和励磁电流间的相位
    // double beta = v - gama;

    // 计算总二次电流
    std::complex<double> i_st;

    i_st = i_ex + i_ct;

    double i_st_abs = std::abs(i_st);

    // 计算一次电流
    double i_p_abs = i_st_abs * n_turns;

    // 计算比差
    double err_ratio = ( i_ct_abs - i_p_abs/n_rated ) / ( i_p_abs/n_rated );

    // 计算Ist和Ict间相位差
    // double theta = atan( i_ex_abs * sin(beta) / ( i_ct_abs + i_ex_abs*cos(beta) ) );
    return err_ratio;
}

bool ctpt_ctrl_p::calculate_inductance( void )
{
//    double phy[3];
//    double cur[3];

//    int np_vol = this->exciting->wave_vol.size();
//    int np_cur = this->exciting->wave_vol.size();


//    if ( np_vol != np_cur )
//    {
//        g_ctpt_log << "np_vol != np_cur:" << np_vol << " vs " << np_cur << "\n";
//        return false;
//    }
//    int len = np_vol - this->knee_index;
//    if ( len <= 0 )
//    {
//        g_ctpt_log << "len <= 0:this->knee_index=" << this->knee_index << "\n";
//        return false;
//    }
//    int step = len / 3;
//    int np = np_vol;
//    if ( step == 0 )
//    {
//        g_ctpt_log << "step == 0:this->knee_index=" << this->knee_index << "\n";
//        return false;
//    }
//    phy[0] = this->exciting->wave_vol[np-1].get_integral_rms();
//    phy[1] = this->exciting->wave_vol[np-1-step].get_integral_rms();
//    phy[2] = this->exciting->wave_vol[np-1-step*2].get_integral_rms();

//    cur[0] = this->exciting->wave_cur[np-1].get_rms();
//    cur[1] = this->exciting->wave_cur[np-1-step].get_rms();
//    cur[2] = this->exciting->wave_cur[np-1-step*2].get_rms();

//    double L12 = ( phy[0] - phy[1] )/ (cur[0] - cur[1]);
//    double L23 = ( phy[1] - phy[2] )/ (cur[1] - cur[2]);
//    double L13 = ( phy[0] - phy[2] )/ (cur[0] - cur[2]);

//    bool ok = true;
//    if ( cur[0] > 5*cur_knee
//         || (0.6 < L12/L23 && L12/L23<1.4)
//         || L13 < 30.0e-3
//        )
//    {
//        ok = false;
//    }
//    result.inductance = inductance; // 饱和电感

    return false;
}


/**********************************************
* @Desc: 负载测试
*
* @Author:zch
* @Date:2016.06.27
*
***********************************************/
int ctpt_ctrl_p::calculate_burden( void )
{

    double output_v = 0.5;
    struct test_result burden_result;
    int ret;
    double Vmeans = 0;
    double Imeans = 0;
    double angle_V_I = 0.0;
    double Itest; // 测试电流

    raw_wave exiting_cur;               // load current
    raw_wave src_vol;               // src voltage
    raw_wave pri_vol;               // primary voltage
    raw_wave sec_vol;               // load voltage

    double Isn = ct_setting.baseParam.secondCurrent;
    if(Isn>5)
        Isn = 5;

    Itest = Isn;

    set_output_freq_np(ct_setting.baseParam.rated_frequency);

    do {
         printf("++set_v %f++\n", output_v);
         if(output_v > MAX_OUT_VOLTE)
         {
            ret = -1;
            break;
         }

         ret = fpga.safe_output(output_v,true);
         if (ret != 0) {
             break;
         }
         ret = fpga.wait_output_steaby();
         if (ret != 0) {
             break;
         }
         ret = fpga.get_active_sample_wave(&exiting_cur, &src_vol, &sec_vol, &pri_vol );
         if (ret != 0) {
             break;
         }
         burden_result.vol = sec_vol._rms_();
         burden_result.cur = exiting_cur._rms_();

         if(fabs(burden_result.cur - Itest) > 0.05)
         {
            if (Itest / burden_result.cur > 2.0)
                output_v *= 2.0;
            else
                output_v += (Itest-burden_result.cur)*(burden_result.vol/burden_result.cur);
         }
         else{
            //接近测试电流，多次采样求平均值
            for(int i =0; i< 5; i++)
            {
                ret = fpga.get_active_sample_wave(&exiting_cur, &src_vol, &sec_vol, & pri_vol);
                if (ret != 0) {
                    break;
                }
                 burden_result.vol = sec_vol._rms_();
                 burden_result.cur = exiting_cur._rms_();
                 burden_result.angle_vol = sec_vol.get_base_angle();
                 burden_result.angle_cur = exiting_cur.get_base_angle();

                Vmeans += burden_result.vol ;
                Imeans += burden_result.cur;
                angle_V_I += burden_result.angle_vol - burden_result.angle_cur;
            }
            Vmeans = Vmeans / 5;
            Imeans = Imeans /5;
            angle_V_I = angle_V_I /5;
            break;
         }
    } while(!is_stopped);

    fpga.stop_output();
    fpga.adc_stop_sample();

     //异常停止
     if(!is_stopped && ret ==-1)
     {
         printf("!!!burden test error!!!!\n");
         return -1;
     }
     double factor = cos(angle_V_I);
     this->result.burden.Z = Vmeans / Imeans;
     this->result.burden.measBurden    = Isn * Isn * this->result.burden.Z;
     this->result.burden.powerFactor	 = (int)(factor*100+0.5)/100.0;//四舍五入
     printf("burden is %f, factro is %f, impedance is %f \n",
            result.burden.measBurden, result.burden.powerFactor, result.burden.Z);
     return 1;
}

bool ctpt_ctrl_p::auto_setup_output( double &/*vol*/, double /*freq*/, double /*vol_min*/, double /*current_max*/, double /*current_need*/ )
{
    bool ok = true;
    return ok;
}
/***************************************************
*   一次电流最大值/额定一次电流.  一次电流的最大值：符合复合误差<10%的一次电流的最大值
*	具体算法：
*	1、	求额定负荷：
*	Zn = Va / Isn^2， 则二次端总负载：Z2 = Rct + Zn
*	2、	求励磁电压电流比
*	Ealf = Alf*Z2*Isn， Iex = Alf*εC*Isn
*	Ealf/Iex = Z2/εC
*	3、	在励磁曲线上查找U/I = Ealf/Iex的电流Iex
*	则Alf = Iex/(εC*Isn)
*****************************************************/
bool ctpt_ctrl_p::calculate_alf_fs( void )
{ 
	// step 1
    double Isn =ct_setting.baseParam.secondCurrent;
    double Zn = ct_setting.baseParam.ratedLoad / (Isn * Isn);
    double Z2 = result.widingRes.refRes + Zn;
	
	// step 2
	// 误差，FS为10%，ALF根据准确级别确定，为5%或10%
	double err_ratio = 0.1;  
	double Iex = 0.0;
	double K = Z2/err_ratio;
	bool ret = find_U_I_ratio(K,Iex);
	double alf = Iex / (err_ratio * Isn);

    switch(ct_setting.baseParam.level) {
    case CTC_MEASURE:
        result.excitation.measure.fs = alf;
        break;
    case CTC_P:
        result.excitation.p.alf = alf;
        break;
    case CTC_PR:
        result.excitation.pr.alf = alf;
        break;
    case CTC_TPX:
        result.excitation.tpx.kssc = alf;
        break;
    case CTC_TPY:
        result.excitation.tpy.kssc = alf;
        break;
    case CTC_TPZ:
        result.excitation.tpz.kssc = alf;
        break;
    case CTC_TPS:
        result.excitation.tps.kssc = alf;
        break;
    default:
        break;
    }

    return ret;
}
/*****************************************
* 查找给定比率的励磁电压电流比对应的电流
* ratio: 比率
* Iex:   该比率对应的电流
* 返回值：找到返回true，否则false，此时用最大电流来算
*******************************************/
bool ctpt_ctrl_p::find_U_I_ratio(double ratio, double& Iex)
{
    if ( this->list_result.size() <2 )
    {
        return false;
    }
    int n = this->list_result.size(); 
    double vol_prev = this->list_result[n-1].vol;	
    double cur_prev = this->list_result[n-1].cur; 
	double ratio_prev = vol_prev / cur_prev;
	
    for (int i=n-2; i>0; i--)
    {
        double vol_next;  
		double cur_next;
		double ratio_next;
        vol_next = this->list_result[i].vol;
		cur_next = this->list_result[i].cur;
		ratio_next = vol_next / cur_next;
		
		double k = (ratio - ratio_prev) * (ratio - ratio_next);
        if ( k < 0 )
        {
            Iex = (cur_next + cur_prev)/2;  
            return true;
        }
        cur_prev = cur_next;
        vol_prev = vol_next;
		ratio_prev = ratio_next;
    }
	Iex = this->list_result[n-1].cur; 
    return false;
	
}

int ctpt_ctrl_p::check_config()
{
    int ret = 0;
    double Isn = ct_setting.baseParam.secondCurrent;
    double maxTestCur = ct_setting.baseParam.testCurrent;
    double freq = ct_setting.baseParam.rated_frequency;
    //限定最大输出电流
    Max_out_current = (maxTestCur>Isn) ? Isn : maxTestCur;

    //工频缺省值为50Hz
    if (fabs(freq-50.0)>0.0001 && fabs(freq-60.0)>0.0001) {
        ct_setting.baseParam.rated_frequency = 50.0;
    }

    return ret;
}

int ctpt_ctrl_p::pre_run_test()
{
    int ret = 0;
    raw_wave loadCurWave, srcVolWave, loadVolWave;

    fpga.reset();
    status_mgr_->set_test_error_code(ERR_SUCCESS);

    set_all_channel_max_range();
    fpga.adc_start_sample();
    fpga.start_output();

//    ret = exciting_out_volte(0.02);
//    if (ret < 0) {
//        status_mgr_->set_test_error_code(::get_error_code()|ERR_AC_SOURCE);
//        printf("[ERR] ctpt_ctrl_p::pre_run_test  exciting_out_volte() < 0\n");
//        goto cleanUp;
//    }
//    ret = fpga.wait_output_steaby();
//    if (ret < 0) {
//        status_mgr_->set_test_error_code(::get_error_code()|ERR_AC_SOURCE);
//        printf("[ERR] ctpt_ctrl_p::pre_run_test  wait_output_steaby() < 0\n");
//        goto cleanUp;
//    }
//    ret = fpga.get_active_sample_wave(&loadCurWave, &srcVolWave, &loadVolWave, 0);
//    if (ret < 0) {
//        status_mgr_->set_test_error_code(::get_error_code());
//        printf("[ERR] ctpt_ctrl_p::pre_run_test  get_active_sample_wave() < 0\n");
//        goto cleanUp;
//    }

//    {
//        //loadCurWave.calculate_all();
//        srcVolWave.calculate_all();
//        loadVolWave.calculate_all();

//        //double angle_cur = radianToAngle(loadCurWave.get_base_angle());
////        double angle_vol = radianToAngle(loadVolWave.get_base_angle());
////        double angle_src = radianToAngle(srcVolWave.get_base_angle());

////        if (fabs(angle_src-angle_vol) > 30) {
////            status_mgr_->set_test_error_code(::get_error_code()|ERR_WIRE_CONNECT_WRONG);
////            ret = -1;
////            printf("[ERR] ctpt_ctrl_p::pre_run_test fabs(angle_src-angle_vol) > 30\n");
////            goto cleanUp;
////        }
//    }

//cleanUp:
    if (ret < 0)
        status_mgr_->set_test_status(TEST_STOP_FOR_EXCEPTION);
    return ret;
}
