#include "ptBaseParamSettings.h"
#include <QDomDocument>
#include <QFile>
#include <QDomDocument>
#include <QDir>
#include <QDebug>

CPTBaseParamSettings::CPTBaseParamSettings()
{
    defaultSettings();
}

CPTBaseParamSettings::~CPTBaseParamSettings()
{

}

bool CPTBaseParamSettings::loadParam(QDomElement* pPTSetELem)
{
    Q_ASSERT(pPTSetELem);

    // 置为默认值
    defaultSettings();

    // 编号
    QDomElement elem = pPTSetELem->firstChildElement("serialNumber");
    if (!elem.isNull()) {
        m_sSettings.number = elem.attribute("val");
    }

    // 绕阻号
    elem = pPTSetELem->firstChildElement("winding");
    if (!elem.isNull()) {
        m_sSettings.winding = elem.attribute("val");
    }

    // 级别
    elem = pPTSetELem->firstChildElement("level");
    if (!elem.isNull()) {
        m_sSettings.level = static_cast<CTPT_CLASS_ENUM>(elem.attribute("val").toUInt());
    }

    // 额定一次电压
    elem = pPTSetELem->firstChildElement("primaryVol");
    if (!elem.isNull()) {
        m_sSettings.primaryVol = elem.attribute("val").toDouble();
    }

    // 额定二次电压
    elem = pPTSetELem->firstChildElement("secondVol");
    if (!elem.isNull()) {
        m_sSettings.secondVol = elem.attribute("val").toDouble();
    }

    // 最大测试电流
    elem = pPTSetELem->firstChildElement("testCur");
    if (!elem.isNull()) {
        m_sSettings.testCur = elem.attribute("val").toDouble();
    }

    // 最大测试电压
    elem = pPTSetELem->firstChildElement("testVol");
    if (!elem.isNull()) {
        m_sSettings.testVol = elem.attribute("val").toDouble();
    }

    // 参考温度
    elem = pPTSetELem->firstChildElement("currentTemp");
    if (!elem.isNull()) {
        m_sSettings.currentTemp = elem.attribute("val").toDouble();
    }

    // 目标温度
    elem = pPTSetELem->firstChildElement("destTemp");
    if (!elem.isNull()) {
        m_sSettings.destTemp = elem.attribute("val").toDouble();
    }

    return true;
}

bool CPTBaseParamSettings::saveParam(QDomDocument* pDoc, QDomElement* pPTSetELem)
{
    Q_ASSERT(pDoc);
    Q_ASSERT(pPTSetELem);

    // 编号
    QDomElement elem = pDoc->createElement("serialNumber");
    elem.setAttribute("val", m_sSettings.number);
    pPTSetELem->appendChild(elem);

    // 绕阻号
    elem = pDoc->createElement("winding");
    elem.setAttribute("val", m_sSettings.winding);
    pPTSetELem->appendChild(elem);

    // 级别
    elem = pDoc->createElement("level");
    elem.setAttribute("val", m_sSettings.level);
    pPTSetELem->appendChild(elem);

    // 额定一次电压
    elem = pDoc->createElement("primaryVol");
    elem.setAttribute("val", m_sSettings.primaryVol);
    pPTSetELem->appendChild(elem);

    // 额定二次电压
    elem = pDoc->createElement("secondVol");
    elem.setAttribute("val", m_sSettings.secondVol);
    pPTSetELem->appendChild(elem);

    // 最大测试电流
    elem = pDoc->createElement("testCur");
    elem.setAttribute("val", m_sSettings.testCur);
    pPTSetELem->appendChild(elem);

    // 最大测试电压
    elem = pDoc->createElement("testVol");
    elem.setAttribute("val", m_sSettings.testVol);
    pPTSetELem->appendChild(elem);

    // 参考温度
    elem = pDoc->createElement("currentTemp");
    elem.setAttribute("val", m_sSettings.currentTemp);
    pPTSetELem->appendChild(elem);

    // 目标温度
    elem = pDoc->createElement("destTemp");
    elem.setAttribute("val", m_sSettings.destTemp);
    pPTSetELem->appendChild(elem);

    return true;
}

void CPTBaseParamSettings::defaultSettings()
{
    m_sSettings.number = "0001";                            // 编号
    m_sSettings.winding = "1S1-1S2";                        // 绕阻号

    m_sSettings.level = CTPT_P;                             // 级别
    m_sSettings.primaryVol = 100.0;                         // 额定一次电压
    m_sSettings.secondVol = 100.0;                          // 额定二次电压

    m_sSettings.testCur = 1.0;                              // 最大测试电流
    m_sSettings.testVol = 1.0;                              // 最大测试电压

    m_sSettings.currentTemp = 75.0;                         // 参考温度
    m_sSettings.destTemp = 50.0;                            // 目标温度
}
