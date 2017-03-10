#ifndef CPTEXTENPARAMSETTINGS_H
#define CPTEXTENPARAMSETTINGS_H

#include "src/CTTester/dataManager/device/paramdef.h"

class QDomDocument;
class QDomElement;

class CPtExtendParamSettings
{
public:
    CPtExtendParamSettings();
    virtual ~CPtExtendParamSettings();

    //S_Pt_Extend_Param ptExtendPatam() const {return m_sSettings;}

#if 0
    // 额定一次电压
    double rated1Voltage() const {return m_sSettings.rated1Voltage;}
    void setRated1Voltage(const double rated1Voltage_) {m_sSettings.rated1Voltage = rated1Voltage_;}
#endif

    // 配置加载/保存
    bool loadParam(QDomElement* pCTSetELem);
    bool saveParam(QDomDocument* pDoc, QDomElement* pCTSetELem);

private:
    void defaultSettings();

private:
    //S_Pt_Extend_Param                   m_sSettings;
};

#endif // CPTEXTENPARAMSETTINGS_H
