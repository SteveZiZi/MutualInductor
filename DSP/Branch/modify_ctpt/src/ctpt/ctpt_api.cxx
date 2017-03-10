// implement test logic
//
// 处理接线方式时分成两种：
//
// 1. 二次输入电压，测量：二次电压电流，一次电压
// 2. 负载电压.
//
// 3. 比差定义:
// 由输入电流Ip
//
#include <assert.h>

#include <iostream>
#include <stdlib.h>
#include <string.h>             // for memset
#include <malloc.h>
#include <unistd.h>
#include <math.h> 
#include <algorithm>
#include "ctpt_api.hpp"
//#include "data_source.hpp"
#include "src/fpga/fpga.hpp" 
#include "src/fpga/raw_wave.hpp"
#include <sstream>
#include <dirent.h>
//#include "data_source_fpga.hpp"

#include "data_type.hpp"
#include "ctpt_ctrl_p.h"
#include "status_mgr.h"

fpga_dev fpga;

using namespace CTPT;


ctpt_result::ctpt_result()
{
    clear();
}

ctpt_result::ctpt_result(const ctpt_result &copyin)
{
    this->type = copyin.type;
    this->level = copyin.level;

    memcpy(&this->widingRes, &copyin.widingRes, sizeof(struct widing_resistance_result));
    memcpy(&this->burden, &copyin.burden, sizeof(struct burden_result));
    memcpy(&this->excitation, &copyin.excitation, sizeof(struct excitation_result));
    memcpy(&this->ratio, &copyin.ratio, sizeof(struct ratio_result));

    this->exciting_data = copyin.exciting_data;
    this->error5_data = copyin.error5_data;
    this->error10_data = copyin.error10_data;
    this->ratio_error_data = copyin.ratio_error_data;
    this->phase_error_data = copyin.phase_error_data;
}

ctpt_result::~ctpt_result()
{
}

ctpt_result &ctpt_result::operator=(const ctpt_result &rhs)
{
    this->type = rhs.type;
    this->level = rhs.level;

    memcpy(&this->widingRes, &rhs.widingRes, sizeof(struct widing_resistance_result));
    memcpy(&this->burden, &rhs.burden, sizeof(struct burden_result));
    memcpy(&this->excitation, &rhs.excitation, sizeof(struct excitation_result));
    memcpy(&this->ratio, &rhs.ratio, sizeof(struct ratio_result));

    this->exciting_data = rhs.exciting_data;
    this->error5_data = rhs.error5_data;
    this->error10_data = rhs.error10_data;
    this->ratio_error_data = rhs.ratio_error_data;
    this->phase_error_data = rhs.phase_error_data;

    return *this;
}

void ctpt_result::clear()
{
    type = TYPE_CT;
    level = CTC_P;

    memset(&this->widingRes, 0, sizeof(struct widing_resistance_result));
    memset(&this->burden, 0, sizeof(struct burden_result));
    memset(&this->excitation, 0, sizeof(struct excitation_result));
    memset(&this->ratio, 0, sizeof(struct ratio_result));
    exciting_data.clear();
    error5_data.clear();
    error10_data.clear();
    ratio_error_data.clear();
    phase_error_data.clear();
}

ctpt_ctrl::ctpt_ctrl()
{
    this->_private = new ctpt_ctrl_p;
}


ctpt_ctrl::~ctpt_ctrl()
{
    if ( this->_private ) {
        delete this->_private;
        this->_private = NULL;
    }
}
 
int ctpt_ctrl::init( void )
{
    return 0;
}

int ctpt_ctrl::reinit( void )
{
    return init();
}


std::string ctpt_ctrl::fpga_ver(void)
{
    return fpga.fpga_version();
}

std::string ctpt_ctrl::ctpt_ver(void)
{
    return fpga.dev_version();
}

int ctpt_ctrl::config( const struct ct_setting &setting )
{
    this->_private->ct_setting = setting;
    this->_private->is_ct_mode = true;
    this->_private->is_pt_mode = false;
    this->_private->user_mode = USER_TEST_RUN;

    return this->_private->check_config();
}

int ctpt_ctrl::config( double voltage, double current, double freq, user_test_mode testMode )
{
    this->_private->is_ct_mode = false;
    this->_private->is_pt_mode = false;
    this->_private->user_mode = testMode;

    this->_private->user_vol = voltage;
    this->_private->user_cur = current;
    this->_private->user_freq = freq;

    std::cout<< "config witch U="<< this->_private->user_vol
             << " I=" << this->_private->user_cur
             << " freq=" << this->_private->user_freq << std::endl;
    return 0;
}

int ctpt_ctrl::config( const struct pt_setting &setting )
{
    int err = 0;

    this->_private->is_ct_mode = false;
    this->_private->is_pt_mode = true;
    this->_private->user_mode = USER_TEST_RUN;
    this->_private->pt_setting = setting;
    return err;
}


int ctpt_ctrl::start( void )
{
    int err = 0;
    std::cout<< "+++start pressed++++" << std::endl;
    if (_private->is_stopped) {
        _private->is_stopped = false;
        StatusMgrInst()->set_test_status(TEST_RUNNING);
        StatusMgrInst()->set_testing();
        sem_post(&this->_private->sem);
    }
    else {
        std::cout<< "[ERR] ctpt_ctrl::start test has started" << std::endl;
        err = -1;
    }

    return err;
}

int ctpt_ctrl::stop( void )
{
    int err = 0;
    std::cout<< "stop pressed" << std::endl;
    this->_private->is_stopped = true;

    return err;
}

int ctpt_ctrl::get_user_test_result( double *voltage, double *current, double *freq_measured )
{
    int err = 0;
    if(voltage){
        *voltage = this->_private->user_result_vol;
    }
    if(current){
        *current = this->_private->user_result_cur;
    }
    if (freq_measured){
        *freq_measured = this->_private->user_freq;
    }
    std::cout<< "get test result witch U="<< this->_private->user_vol << " freq=" << this->_private->user_freq
             << " I=" << this->_private->user_cur << std::endl;
    return err;
}

int ctpt_ctrl::get_ctpt_result(ctpt_result &out)
{
    printf("get ctpt result\n");

    out = _private->result;

    if (_private->is_ct_mode) {
        out.type = TYPE_CT;
        out.level = _private->ct_setting.baseParam.level;
    }
    else {
        out.type = TYPE_PT;
        out.level = _private->pt_setting.level;
    }
    
    return 0;
}
	
	
	
