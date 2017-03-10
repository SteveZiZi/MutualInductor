#ifndef CTBASEPARAMSETTTINGS_H
#define CTBASEPARAMSETTTINGS_H

#include "src/CTTester/dataManager/device/paramdef.h"

class QDomDocument;
class QDomElement;

class CCTBaseParamSettings
{
public:
    CCTBaseParamSettings();
    virtual ~CCTBaseParamSettings();

    const S_CT_Base_Param &param() const {return m_sSettings;}

    // CT编号
    QString CtNum() const {return m_sSettings.serialNumber;}
    void setCtNum(const QString & CtNum_) {m_sSettings.serialNumber = CtNum_;}

    // 绕阻号
    QString windingNum() const {return m_sSettings.winding;}
    void setWindingNum(const QString & windingNum_) {m_sSettings.winding = windingNum_;}

    // 级别
    int testLevel() const {return m_sSettings.level;}
    void setTestLevel(const int level_) {m_sSettings.level = static_cast<CTPT_CLASS_ENUM>(level_);}

    // 最大测试电流
    double maxTestCurrent() const {return m_sSettings.testCurrent;}
    void setMaxTestCurrent(const double testCurrent_) {m_sSettings.testCurrent = testCurrent_;}

    // 额定一次电流
    double primaryCurrent() const {return m_sSettings.primaryCurrent;}
    void setprimaryCurrent(const double _1Current) {m_sSettings.primaryCurrent = _1Current;}

    // 额定二次电流
    double secondCurrent() const {return m_sSettings.secondCurrent;}
    void setSecondCurrent(const double _2Current) {m_sSettings.secondCurrent = _2Current;}

    // 额定负荷
    double ratedLoad() const {return m_sSettings.ratedLoad;}
    void setRatedLoad(const double ratedLoad_) {m_sSettings.ratedLoad = ratedLoad_;}

    // 功率因数
    double powerFactor() const {return m_sSettings.powerFactor;}
    void setPowerFactor(const double powerFactor_) {m_sSettings.powerFactor = powerFactor_;}

    // 参考温度
    double currentTemp() const {return m_sSettings.currentTemp;}
    void setCurrentTemp(const double currentTemp_) {m_sSettings.currentTemp = currentTemp_;}

    // 目标温度
    double destTemp() const {return m_sSettings.destTemp;}
    void setDestTemp(const double destTemp_) {m_sSettings.destTemp = destTemp_;}

#if 0
    // 线路号
    QString lineNum() const {return m_sSettings.comBaseParam.lineNum;}
    void setLineNum(const QString & lineNum_) {m_sSettings.comBaseParam.lineNum = lineNum_;}

    // 相别
    QString phaseNum() const {return m_sSettings.comBaseParam.phase;}
    void setPhaseNum(const QString & phaseNum_) {m_sSettings.comBaseParam.phase = phaseNum_;}

    // 直阻
    double DcResistance() const {return m_sSettings.comBaseParam.DcResistance;}
    void setDcResistance(const double DcResistance_) {m_sSettings.comBaseParam.DcResistance = DcResistance_;}

    // 频率
    double frequency() const {return m_sSettings.comBaseParam.frequency;}
    void setFrequency(const double frequency_) {m_sSettings.comBaseParam.frequency = frequency_;}
#endif

    // 配置加载/保存
    bool loadParam(QDomElement* pCTSetELem);
    bool saveParam(QDomDocument* pDoc, QDomElement* pCTSetELem);

private:
    void defaultSettings();

private:
    S_CT_Base_Param             m_sSettings;
};

#endif // CTBASEPARAMSETTTINGS_H

