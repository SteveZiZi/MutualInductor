#include "resultParamSettings.h"

#include "src/CTTester/dataManager/device/ctpt_def.h"

CResultParamSettings::CResultParamSettings()
{
    if(!loadSettingsFromFile())
    {
        defaultSettings();
    }
}

CResultParamSettings::~CResultParamSettings()
{

}

bool CResultParamSettings::loadParam()
{
    return true;
}

bool CResultParamSettings::saveParam()
{
    return true;
}

CTPT::ui_wave_data exicting_data[] = {
    {0.005839, 1.031052},
    {0.009185, 1.993365},
    {0.012009, 2.954451},
    {0.014588, 3.955039},
    {0.017119, 4.932807},
    {0.019554, 5.918238},
    {0.023342, 7.065898},
    {0.025981, 8.036549},
    {0.028667, 9.01074},
    {0.032516, 9.964093},
    {0.037412, 10.91861},
    {0.044367, 11.87869},
    {0.054530, 12.83005},
    {0.069509, 13.76534},
    {0.096006, 14.75045},
    {0.143921, 15.62551},
    {0.235834, 16.60475},
    {0.465969, 17.53329},
    {0.882785, 18.45501},
    {1.583006, 19.29834}
};

void CResultParamSettings::defaultSettings()
{
    //负荷
    m_sSettings.burden.measBurden = 15.0;
    m_sSettings.burden.powerFactor = 0.8;
    m_sSettings.burden.Z = 15.0;
    // 直阻
    m_sSettings.widingRes.measRes = 20.0;
    m_sSettings.widingRes.refRes = (234.5+75)/(234.5+25)*m_sSettings.widingRes.measRes;

    // 励磁
    m_sSettings.excitation.Ikn = 0.062;
    m_sSettings.excitation.Vkn = 165.0;
    m_sSettings.excitation.Lu  = 41.3;
    m_sSettings.excitation.Kr  = 77.5;
    m_sSettings.excitation.Ts  = 55.6;

    // 变比
    m_sSettings.ratio.ratio = 2999.5;
    m_sSettings.ratio.ratio_error = 0.15;
    m_sSettings.ratio.phase_error = 1.5;
    m_sSettings.ratio.turns_ratio = 3000;
    m_sSettings.ratio.turns_ratio_error = 0.01;
    m_sSettings.ratio.phase_polar = true;


    // 励磁数据
    for(unsigned int i=0; i<sizeof(exicting_data)/sizeof(exicting_data[0]); i++)
        m_sSettings.exciting_data.push_back(exicting_data[i]);

    // 5%误差曲线

    // 10%误差曲线

    // 角差数据
    CTPT::ratio_error_data phaseData;
    phaseData.VA = 15;
    phaseData.factor = 0.8;
    phaseData.percent1 = 13.42;
    phaseData.percent5 = 5.6;
    phaseData.percent20 = 2.88;
    phaseData.percent100 = 1.46;
    phaseData.percent120 = 1.38;
    m_sSettings.phase_error_data.push_back(phaseData);

    phaseData.VA = 7.5;
    phaseData.factor = 0.8;
    m_sSettings.phase_error_data.push_back(phaseData);

    phaseData.VA = 3.25;
    phaseData.factor = 0.8;
    m_sSettings.phase_error_data.push_back(phaseData);

    phaseData.VA = 1;
    phaseData.factor = 1.0;
    m_sSettings.phase_error_data.push_back(phaseData);

    // 比差数据
    CTPT::ratio_error_data errData;
    errData.VA = 15;
    errData.factor = 0.8;
    errData.percent1 = -0.063;
    errData.percent5 = -0.033;
    errData.percent20 = -0.005;
    errData.percent100 = 0.013;
    errData.percent120 = 0.016;
    m_sSettings.ratio_error_data.push_back(errData);

    errData.VA = 7.5;
    errData.factor = 0.8;
    m_sSettings.ratio_error_data.push_back(errData);

    errData.VA = 3.25;
    errData.factor = 0.8;
    m_sSettings.ratio_error_data.push_back(errData);

    errData.VA = 1;
    errData.factor = 1.0;
    m_sSettings.ratio_error_data.push_back(errData);
}

bool CResultParamSettings::loadSettingsFromFile()
{
    return false;
}

