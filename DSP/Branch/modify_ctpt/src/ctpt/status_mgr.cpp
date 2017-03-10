#include "status_mgr.h"

#include "src/fpga/common.h"
#include "src/inc/auto_guard.h"
#include "src/inc/tmutex.h"
#include "src/fpga/fpga.hpp"
#include "data_type.hpp"

extern fpga_dev fpga;

using namespace CTPT;

class CTPT::status_mgr_d
{
public:
    status_mgr_d();


    double load_cur;
    double load_vol;
    double pri_vol;

    TEST_STATUS_ENUM test_status;
    uint32_t err_code;
    ctpt_test_item test_item;

    bool m_isTesting;

    struct ThreadMutex thread_mutex;

};
status_mgr_d::status_mgr_d() :
    load_cur(0),
    load_vol(0),
    pri_vol(0),
    test_status(TEST_NOT_START),
    err_code(ERR_SUCCESS),
    m_isTesting(false)
{

}



/***************************************************************
 * *        status_mgr
 * ************************************************************/
status_mgr::status_mgr()
{
    m_data = new status_mgr_d();
}



CTPT::status_mgr::~status_mgr()
{

}

bool CTPT::status_mgr::isTesting()
{
    AUTO_GUARD(guard, ThreadMutex, m_data->thread_mutex);
    return m_data->m_isTesting;
}

double status_mgr::get_load_cur()
{
//    AUTO_GUARD(guard, ThreadMutex, m_data->thread_mutex);
//    return m_data->load_cur;
    return fpga.get_load_current();
}
double status_mgr::get_load_vol()
{
//    AUTO_GUARD(guard, ThreadMutex, m_data->thread_mutex);
//    return m_data->load_vol;
    return fpga.get_load_vol();
}
double status_mgr::get_pri_vol()
{
//    AUTO_GUARD(guard, ThreadMutex, m_data->thread_mutex);
//    return m_data->pri_vol;
    return fpga.get_pri_vol();
}

TEST_STATUS_ENUM status_mgr::get_test_status()
{
    AUTO_GUARD(guard, ThreadMutex, m_data->thread_mutex);
    return m_data->test_status;
}

uint32_t status_mgr::get_test_error_code()
{
    AUTO_GUARD(guard, ThreadMutex, m_data->thread_mutex);
    return m_data->err_code;
}

std::string status_mgr::error_code_to_string(uint32_t err_code)
{
    uint32_t hardware_code = err_code & ERR_CODE_HARDWARE_MASK;
    std::string strErr = ::error_code_to_string(hardware_code);
    uint32_t user_code = err_code & ERR_CODE_USER_MASK;

    // TOO: Add user error code
    switch (user_code) {
    case ERR_NOT_FOUND__KNEE_POINT:
        strErr += "没有找到拐点\n";
        break;
    case ERR_WIRE_CONNECT_WRONG:
        strErr += "接线错误\n";
        break;
    default:
        strErr += "未定义错误\n";
        break;
    }

    return strErr;
}

ctpt_test_item status_mgr::get_current_test_item()
{
    AUTO_GUARD(guard, ThreadMutex, m_data->thread_mutex);
    return m_data->test_item;
}

void status_mgr::set_load_cur(double cur)
{
    AUTO_GUARD(guard, ThreadMutex, m_data->thread_mutex);
    m_data->load_cur = cur;
}
void status_mgr::set_load_vol(double vol)
{
    AUTO_GUARD(guard, ThreadMutex, m_data->thread_mutex);
    m_data->load_vol = vol;
}
void status_mgr::set_pri_vol(double vol)
{
    AUTO_GUARD(guard, ThreadMutex, m_data->thread_mutex);
    m_data->pri_vol = vol;
}

void status_mgr::set_test_status(TEST_STATUS_ENUM status)
{
    AUTO_GUARD(guard, ThreadMutex, m_data->thread_mutex);
    m_data->test_status = status;
}

void status_mgr::set_test_error_code(uint32_t err_code)
{
    AUTO_GUARD(guard, ThreadMutex, m_data->thread_mutex);
    m_data->err_code = err_code;
}

void status_mgr::set_current_test_item(ctpt_test_item ti)
{
    AUTO_GUARD(guard, ThreadMutex, m_data->thread_mutex);
    m_data->test_item = ti;
}

void status_mgr::set_testing(bool testing)
{
    AUTO_GUARD(guard, ThreadMutex, m_data->thread_mutex);
    m_data->m_isTesting = testing;
}

status_mgr *CTPT::StatusMgrInst()
{
    static status_mgr mgr;

    return &mgr;
}
