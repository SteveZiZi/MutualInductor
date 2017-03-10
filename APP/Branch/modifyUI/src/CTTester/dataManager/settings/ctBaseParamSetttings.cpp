#include "ctBaseParamSetttings.h"
#include <QDomDocument>
#include <QFile>
#include <QDir>
#include <QDebug>

CCTBaseParamSettings::CCTBaseParamSettings()
{
    defaultSettings();
}

CCTBaseParamSettings::~CCTBaseParamSettings()
{

}

bool CCTBaseParamSettings::loadParam(QDomElement* pCTSetELem)
{
    Q_ASSERT(pCTSetELem);

    // 置为默认值
    defaultSettings();

    // 编号
    QDomElement elem = pCTSetELem->firstChildElement("serialNumber");
    if (!elem.isNull()) {
        m_sSettings.serialNumber = elem.attribute("val");
    }

    // 绕阻号
    elem = pCTSetELem->firstChildElement("winding");
    if (!elem.isNull()) {
        m_sSettings.winding = elem.attribute("val");
    }

    // 级别
    elem = pCTSetELem->firstChildElement("level");
    if (!elem.isNull()) {
        m_sSettings.level = static_cast<CTPT_CLASS_ENUM>(elem.attribute("val").toUInt());
    }

    // 最大测试电流
    elem = pCTSetELem->firstChildElement("testCurrent");
    if (!elem.isNull()) {
        m_sSettings.testCurrent = elem.attribute("val").toDouble();
    }

    // 额定一次电流
    elem = pCTSetELem->firstChildElement("primaryCurrent");
    if (!elem.isNull()) {
        m_sSettings.primaryCurrent = elem.attribute("val").toDouble();
    }

    // 额定二次电流
    elem = pCTSetELem->firstChildElement("secondCurrent");
    if (!elem.isNull()) {
        m_sSettings.secondCurrent = elem.attribute("val").toDouble();
    }

    // 额定负荷
    elem = pCTSetELem->firstChildElement("ratedLoad");
    if (!elem.isNull()) {
        m_sSettings.ratedLoad = elem.attribute("val").toDouble();
    }

    // 功率因数
    elem = pCTSetELem->firstChildElement("powerFactor");
    if (!elem.isNull()) {
        m_sSettings.powerFactor = elem.attribute("val").toDouble();
    }

    // 参考温度
    elem = pCTSetELem->firstChildElement("currentTemp");
    if (!elem.isNull()) {
        m_sSettings.currentTemp = elem.attribute("val").toDouble();
    }

    // 目标温度
    elem = pCTSetELem->firstChildElement("destTemp");
    if (!elem.isNull()) {
        m_sSettings.destTemp = elem.attribute("val").toDouble();
    }

    return true;
}

bool CCTBaseParamSettings::saveParam(QDomDocument* pDoc, QDomElement* pCTSetELem)
{
    Q_ASSERT(pDoc);
    Q_ASSERT(pCTSetELem);

    // 编号
    QDomElement elem = pDoc->createElement("serialNumber");
    elem.setAttribute("val", m_sSettings.serialNumber);
    pCTSetELem->appendChild(elem);

    // 绕阻号
    elem = pDoc->createElement("winding");
    elem.setAttribute("val", m_sSettings.winding);
    pCTSetELem->appendChild(elem);

    // 级别
    elem = pDoc->createElement("level");
    elem.setAttribute("val", m_sSettings.level);
    pCTSetELem->appendChild(elem);

    // 最大测试电流
    elem = pDoc->createElement("testCurrent");
    elem.setAttribute("val", m_sSettings.testCurrent);
    pCTSetELem->appendChild(elem);

    // 额定一次电流
    elem = pDoc->createElement("primaryCurrent");
    elem.setAttribute("val", m_sSettings.primaryCurrent);
    pCTSetELem->appendChild(elem);

    // 额定二次电流
    elem = pDoc->createElement("secondCurrent");
    elem.setAttribute("val", m_sSettings.secondCurrent);
    pCTSetELem->appendChild(elem);

    // 额定负荷
    elem = pDoc->createElement("ratedLoad");
    elem.setAttribute("val", m_sSettings.ratedLoad);
    pCTSetELem->appendChild(elem);

    // 功率因数
    elem = pDoc->createElement("powerFactor");
    elem.setAttribute("val", m_sSettings.powerFactor);
    pCTSetELem->appendChild(elem);

    // 参考温度
    elem = pDoc->createElement("currentTemp");
    elem.setAttribute("val", m_sSettings.currentTemp);
    pCTSetELem->appendChild(elem);

    // 目标温度
    elem = pDoc->createElement("destTemp");
    elem.setAttribute("val", m_sSettings.destTemp);
    pCTSetELem->appendChild(elem);

    return true;
}

void CCTBaseParamSettings::defaultSettings()
{
    m_sSettings.serialNumber = "0001";                      // 编号
    m_sSettings.winding = "1S1-1S2";                        // 绕阻号

    m_sSettings.level = CTPT_P;                             // 级别
    m_sSettings.testCurrent = 1.0;                          // 最大测试电流
    m_sSettings.primaryCurrent = 3000.0;                       // 额定一次电流
    m_sSettings.secondCurrent = 1.0;                        // 额定二次电流

    m_sSettings.ratedLoad = 15.0;                           // 额定负荷
    m_sSettings.powerFactor = 0.8;                          // 功率因数

    m_sSettings.currentTemp = 25.0;                         // 参考温度
    m_sSettings.destTemp = 75.0;                            // 目标温度
}
