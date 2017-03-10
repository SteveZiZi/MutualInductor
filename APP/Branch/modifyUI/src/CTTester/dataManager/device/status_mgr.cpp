#include "status_mgr.h"


using namespace CTPT;

extern bool g_isTesting;
extern TEST_STATUS_ENUM g_testStatus;
extern ctpt_test_item   g_testItem;


status_mgr::status_mgr()
{

}

status_mgr::~status_mgr()
{

}

bool status_mgr::isTesting()
{
    return g_isTesting;
}

double status_mgr::get_load_cur()
{
    return 0.001;
}

double status_mgr::get_load_vol()
{
    return 0.1;
}

double status_mgr::get_pri_vol()
{
    return 0.001;
}

TEST_STATUS_ENUM status_mgr::get_test_status()
{
    return g_testStatus;
}

uint32_t status_mgr::get_test_error_code()
{
    return 0x00;
}

std::string status_mgr::error_code_to_string(uint32_t /*err_code*/)
{
    return "no error";
}

ctpt_test_item status_mgr::get_current_test_item()
{
    return g_testItem;
}

void status_mgr::set_load_cur(double /*cur*/)
{

}

void status_mgr::set_load_vol(double /*vol*/)
{

}

void status_mgr::set_pri_vol(double /*vol*/)
{

}

void status_mgr::set_test_status(TEST_STATUS_ENUM /*status*/)
{

}

void status_mgr::set_test_error_code(uint32_t /*err_code*/)
{

}

void status_mgr::set_current_test_item(ctpt_test_item /*ti*/)
{
}

void status_mgr::set_testing(bool /*testing*/)
{
}

CTPT::status_mgr *CTPT::StatusMgrInst()
{
    static status_mgr mgr;

    return &mgr;
}
