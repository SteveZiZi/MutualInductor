#ifndef STATUS_MGR_H
#define STATUS_MGR_H

#include "ctpt_def.h"
#include <stdint.h>
#include <string>

namespace CTPT {

enum TEST_STATUS_ENUM
{
    TEST_RUNNING,                       //  测试正常进行
    TEST_STOP_FOR_USER_CANCEL,          //  用户取消测试
    TEST_STOP_FOR_EXCEPTION,            //  出现异常，停止测试
    TEST_FINISHED,                      //  测试正常，完成测试
    TEST_NOT_START                      //  并未开始测试，系统刚启动时的状态
};


class status_mgr_d;
class status_mgr
{
    friend class ctpt_ctrl_p;
    friend class ctpt_ctrl;
public:
    CPTAPI status_mgr();
    CPTAPI ~status_mgr();

    /*
     *  @Func:  isTesting
     *  @Return:
     *     true:   测试正常进行
     *     false:  测试停止，停止原因：测试完成、用户停止测试、出现异常停止测试
     *  @Note:
     *     通过获取测试错误代码获取测试停止的原因
     */
    CPTAPI bool isTesting();

    CPTAPI double get_load_cur();
    CPTAPI double get_load_vol();
    CPTAPI double get_pri_vol();

    CPTAPI TEST_STATUS_ENUM get_test_status();
    CPTAPI uint32_t get_test_error_code();
    CPTAPI std::string error_code_to_string(uint32_t err_code);

    CPTAPI ctpt_test_item get_current_test_item();

private:
    void set_load_cur(double cur);
    void set_load_vol(double vol);
    void set_pri_vol(double vol);

    void set_test_status(TEST_STATUS_ENUM status);
    void set_test_error_code(uint32_t err_code);
    void set_current_test_item(ctpt_test_item ti);

    void set_testing(bool testing = true);

private:
    status_mgr_d * m_data;
};

CPTAPI status_mgr* StatusMgrInst();

}

#endif // STATUS_MGR_H
