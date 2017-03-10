#pragma once

#include "private_def.h"

class calibrate_dev;

class CAdcCalibration
{
public:
    static CAdcCalibration* getInstance();

    int init();

    bool chnl_is_valid(int chnl);
    const ad_calibrate_result* result(int chnl);
    int setResult(int chnl, const ad_calibrate_result &result_);
    ad_calibrate_result *get_calibrate_result(int chnl);

    void saveResult();

    void dumpRange();

protected:
    CAdcCalibration();
    ~CAdcCalibration();

private:

    void format_content1();
    void format_content2();

private:
    calibrate_dev * m_dev[2];
    bool m_resultChanged[2];

    ad_calibrate_result *adCalibrateResult[MAX_CHNL_NUM];
};

