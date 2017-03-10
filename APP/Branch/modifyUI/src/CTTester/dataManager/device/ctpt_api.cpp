#include "ctpt_api.hpp"

#include <QDebug>

#include "src/CTTester/dataManager/settings/resultParamSettings.h"
#include "status_mgr.h"

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


#include <QRunnable>
#include <QThreadPool>

bool g_isTesting = false;
TEST_STATUS_ENUM g_testStatus = TEST_NOT_START;
ctpt_test_item   g_testItem = ITEM_RESISTANCE;


class runnableInst : public QRunnable
{
public:
    runnableInst() : QRunnable() {}

    void run();
};

void runnableInst::run()
{
    qDebug() << "stat testing";
    g_testStatus = TEST_RUNNING;

    g_testItem = ITEM_RESISTANCE;
    QThread::sleep(2);

    if (g_isTesting) {
        g_testItem = ITEM_EXCITATION;
        QThread::sleep(2);
    }
    if (g_isTesting) {
        g_testItem = ITEM_RATIO;
        QThread::sleep(2);
    }
    if (g_isTesting) {
        g_testItem = ITEM_DEMAGNETIOZATION;
        QThread::sleep(2);
    }

    if (g_isTesting) {
        g_testStatus = TEST_FINISHED;
        g_isTesting = false;
        qDebug() << "test Finished";
    }
}


ctpt_ctrl::ctpt_ctrl()
{

}

ctpt_ctrl::~ctpt_ctrl()
{

}

int ctpt_ctrl::init()
{
    qDebug() << "CTPT::ctpt_ctrl::init";
    return 0;
}

int ctpt_ctrl::reinit()
{
    return init();
}

std::string ctpt_ctrl::fpga_ver()
{
    return "1.0.00";
}

std::string ctpt_ctrl::ctpt_ver()
{
    return "1.1.00";
}

int ctpt_ctrl::config(const ct_setting &)
{
    qDebug() << "ctpt_ctrl::config ct_setting";
    return 0;
}

int ctpt_ctrl::config(const pt_setting &)
{
    qDebug() << "ctpt_ctrl::config pt_setting";
    return 0;
}

int ctpt_ctrl::config(double /*voltage*/,
                      double /*current*/,
                      double /*freq*/,
                      user_test_mode /*testMode*/)
{
    qDebug() << "ctpt_ctrl::config user_test_mode";
    return 0;
}

int ctpt_ctrl::start()
{
    qDebug() << "ctpt_ctrl::start";
    g_isTesting = true;
    runnableInst* hInst = new runnableInst();
    QThreadPool::globalInstance()->start(hInst);
    return 0;
}

int ctpt_ctrl::stop()
{
    qDebug() << "ctpt_ctrl::stop";
    g_isTesting = false;
    g_testStatus = TEST_STOP_FOR_USER_CANCEL;
    return 0;
}

int ctpt_ctrl::get_ctpt_result(ctpt_result &out)
{
    CResultParamSettings result;
    out = result.ctTpResult();
    return 0;
}

int ctpt_ctrl::get_user_test_result(double */*voltage*/, double */*current*/, double */*freq_measured*/)
{
    qDebug() << "ctpt_ctrl::get_user_test_result";
    return 0;
}
