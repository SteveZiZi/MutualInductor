#ifndef CPTBASEPARAMSETTINGS_H
#define CPTBASEPARAMSETTINGS_H

#include "src/CTTester/dataManager/device/paramdef.h"

class QDomDocument;
class QDomElement;

class CPTBaseParamSettings
{
public:
    CPTBaseParamSettings();
    virtual ~CPTBaseParamSettings();

    const S_PT_Base_Param &param() const {return m_sSettings;}

    // PT编号
    QString PtNum() const {return m_sSettings.number;}
    void setPtNum(const QString & PtNum_) {m_sSettings.number = PtNum_;}

    // 绕阻号
    QString windingNum() const {return m_sSettings.winding;}
    void setWindingNum(const QString & windingNum_) {m_sSettings.winding = windingNum_;}

    // 级别
    int testLevel() const {return m_sSettings.level;}
    void setTestLevel(const int level_) {m_sSettings.level = static_cast<CTPT_CLASS_ENUM>(level_);}

    // 额定一次电压
    double primaryVol() const {return m_sSettings.primaryVol;}
    void setprimaryVol(const double _1Volgate) {m_sSettings.primaryVol = _1Volgate;}

    // 额定二次电流
    double secondVol() const {return m_sSettings.secondVol;}
    void setSecondVol(const double _2Volgate) {m_sSettings.secondVol = _2Volgate;}

    // 最大测试电流
    double maxTestCurrent() const {return m_sSettings.testCur;}
    void setMaxTestCurrent(const double testCurrent_) {m_sSettings.testCur = testCurrent_;}

    // 最大测试电压
    double maxTestVol() const {return m_sSettings.testVol;}
    void setMaxTestVol(const double testVolgate_) {m_sSettings.testVol = testVolgate_;}

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
    bool loadParam(QDomElement* pPTSetELem);
    bool saveParam(QDomDocument* pDoc, QDomElement* pPTSetELem);

private:
    void defaultSettings();

private:
    S_PT_Base_Param         m_sSettings;
};

#endif // CPTBASEPARAMSETTINGS_H
