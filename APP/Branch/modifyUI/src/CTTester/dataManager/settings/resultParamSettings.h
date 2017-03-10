#ifndef CRESULTPARAMSETTINGS_H
#define CRESULTPARAMSETTINGS_H

#include "src/CTTester/dataManager/device/paramdef.h"

class CResultParamSettings
{
public:
    CResultParamSettings();
    virtual ~CResultParamSettings();

    const S_CTPT_Result & ctTpResult() const {return m_sSettings;}

    bool loadParam();
    bool saveParam();

private:
    void defaultSettings();
    //  从配置文件加载配置
    bool loadSettingsFromFile();

private:
    S_CTPT_Result             m_sSettings;
};

#endif // CRESULTPARAMSETTINGS_H
