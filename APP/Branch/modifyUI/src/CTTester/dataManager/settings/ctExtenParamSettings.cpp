#include "ctExtenParamSettings.h"
#include <QDomDocument>
#include <QFile>
#include <QDomDocument>
#include <QDir>
#include <QDebug>

/*************************************
 * CCTExtendParamSettings_Measure
 * ***********************************/

CCTExtendParamSettings_Measure::CCTExtendParamSettings_Measure()
{
    defaultSettings();
}

CCTExtendParamSettings_Measure::~CCTExtendParamSettings_Measure()
{

}

bool CCTExtendParamSettings_Measure::loadParam(QDomElement* pCTSetELem)
{
    Q_ASSERT(pCTSetELem);

    // 置为默认值
    defaultSettings();

    // 额定仪表保安系数
    QDomElement elem = pCTSetELem->firstChildElement("fs");
    if (!elem.isNull()) {
        m_sSettings.fs = elem.attribute("val").toDouble();
    }

    // 扩展电流标定
    elem = pCTSetELem->firstChildElement("extendCurrent");
    if (!elem.isNull()) {
        m_sSettings.extendCurrent = elem.attribute("val").toDouble();
    }

    return true;
}

bool CCTExtendParamSettings_Measure::saveParam(QDomDocument* pDoc, QDomElement* pCTSetELem)
{
    Q_ASSERT(pDoc);
    Q_ASSERT(pCTSetELem);

    // 编号
    QDomElement elem = pDoc->createElement("fs");
    elem.setAttribute("val", m_sSettings.fs);
    pCTSetELem->appendChild(elem);

    // 绕阻号
    elem = pDoc->createElement("extendCurrent");
    elem.setAttribute("val", m_sSettings.extendCurrent);
    pCTSetELem->appendChild(elem);

    return true;
}

void CCTExtendParamSettings_Measure::defaultSettings()
{
    m_sSettings.fs = 15.0;                             // 额定仪表保安系数
    m_sSettings.extendCurrent = 1.2;                   // 扩展电流标定
}


/*************************************
 * CCTExtendParamSettings_P
 * ***********************************/

CCTExtendParamSettings_P::CCTExtendParamSettings_P()
{
    defaultSettings();
}

CCTExtendParamSettings_P::~CCTExtendParamSettings_P()
{

}

bool CCTExtendParamSettings_P::loadParam(QDomElement* pCTSetELem)
{
    Q_ASSERT(pCTSetELem);

    // 置为默认值
    defaultSettings();

    // 额定准确限值系数
    QDomElement elem = pCTSetELem->firstChildElement("alf");
    if (!elem.isNull()) {
        m_sSettings.alf = elem.attribute("val").toDouble();
    }

    return true;
}

bool CCTExtendParamSettings_P::saveParam(QDomDocument* pDoc, QDomElement* pCTSetELem)
{
    Q_ASSERT(pDoc);
    Q_ASSERT(pCTSetELem);

    // 编号
    QDomElement elem = pDoc->createElement("alf");
    elem.setAttribute("val", m_sSettings.alf);
    pCTSetELem->appendChild(elem);

    return true;
}

void CCTExtendParamSettings_P::defaultSettings()
{
    m_sSettings.alf = 15;
}


/*************************************
 * CCTExtendParamSettings_PR
 * ***********************************/

CCTExtendParamSettings_PR::CCTExtendParamSettings_PR()
{
    defaultSettings();
}

CCTExtendParamSettings_PR::~CCTExtendParamSettings_PR()
{

}

bool CCTExtendParamSettings_PR::loadParam(QDomElement* pCTSetELem)
{
    Q_ASSERT(pCTSetELem);

    // 置为默认值
    defaultSettings();

    // 额定准确限值系数
    QDomElement elem = pCTSetELem->firstChildElement("alf");
    if (!elem.isNull()) {
        m_sSettings.alf = elem.attribute("val").toDouble();
    }

    // 二次时间常数
    elem = pCTSetELem->firstChildElement("Ts");
    if (!elem.isNull()) {
        m_sSettings.Ts = elem.attribute("val").toDouble();
    }

    return true;
}

bool CCTExtendParamSettings_PR::saveParam(QDomDocument* pDoc, QDomElement* pCTSetELem)
{
    Q_ASSERT(pDoc);
    Q_ASSERT(pCTSetELem);

    // 额定准确限值系数
    QDomElement elem = pDoc->createElement("alf");
    elem.setAttribute("val", m_sSettings.alf);
    pCTSetELem->appendChild(elem);

    // 二次时间常数
    elem = pDoc->createElement("Ts");
    elem.setAttribute("val", m_sSettings.Ts);
    pCTSetELem->appendChild(elem);

    return true;
}

void CCTExtendParamSettings_PR::defaultSettings()
{
    m_sSettings.alf = 15.0;                            // 额定准确限值系数
    m_sSettings.Ts = 100.0;                             // 二次时间常数
}


/*************************************
 * CCTExtendParamSettings_PX
 * ***********************************/

#define     PX_LEVEL_PARAM_SETTINGS_FILE            ".\\config\\CCTExtendParamSettings_PX.xml"
#define     PX_LEVEL_PARAM_SETTINGS_FILE_NAME       ".\\user\\config\\CCTExtendParamSettings_PX.xml"

CCTExtendParamSettings_PX::CCTExtendParamSettings_PX()
{
    defaultSettings();
}

CCTExtendParamSettings_PX::~CCTExtendParamSettings_PX()
{

}

bool CCTExtendParamSettings_PX::loadParam(QDomElement* pCTSetELem)
{
    Q_ASSERT(pCTSetELem);

    // 置为默认值
    defaultSettings();

    // 额定计算系数Kx
    QDomElement elem = pCTSetELem->firstChildElement("Kx");
    if (!elem.isNull()) {
        m_sSettings.Kx = elem.attribute("val").toDouble();
    }

    // 额定拐点电势
    elem = pCTSetELem->firstChildElement("Ek");
    if (!elem.isNull()) {
        m_sSettings.Ek = elem.attribute("val").toDouble();
    }

    // Ek对应le
    elem = pCTSetELem->firstChildElement("Ie");
    if (!elem.isNull()) {
        m_sSettings.Ie = elem.attribute("val").toDouble();
    }

    return true;
}

bool CCTExtendParamSettings_PX::saveParam(QDomDocument* pDoc, QDomElement* pCTSetELem)
{
    Q_ASSERT(pDoc);
    Q_ASSERT(pCTSetELem);

    // 额定计算系数Kx
    QDomElement elem = pDoc->createElement("Kx");
    elem.setAttribute("val", m_sSettings.Kx);
    pCTSetELem->appendChild(elem);

    // 额定拐点电势
    elem = pDoc->createElement("Ek");
    elem.setAttribute("val", m_sSettings.Ek);
    pCTSetELem->appendChild(elem);

    // Ek对应le
    elem = pDoc->createElement("Ie");
    elem.setAttribute("val", m_sSettings.Ie);
    pCTSetELem->appendChild(elem);

    return true;
}

void CCTExtendParamSettings_PX::defaultSettings()
{
    m_sSettings.Kx = 15.0;                              // 额定计算系数Kx
    m_sSettings.Ek = 100.0;                             // 额定拐点电势
    m_sSettings.Ie = 0.0;                               // Ek对应Ie
}


/*************************************
 * CCTExtendParamSettings_TPX
 * ***********************************/

CCTExtendParamSettings_TPX::CCTExtendParamSettings_TPX()
{
    defaultSettings();
}

CCTExtendParamSettings_TPX::~CCTExtendParamSettings_TPX()
{

}

bool CCTExtendParamSettings_TPX::loadParam(QDomElement* pCTSetELem)
{
    Q_ASSERT(pCTSetELem);

    // 置为默认值
    defaultSettings();

    // 额定对称短路电流系数
    QDomElement elem = pCTSetELem->firstChildElement("kssc");
    if (!elem.isNull()) {
        m_sSettings.kssc = elem.attribute("val").toDouble();
    }

    // 暂态面积系数
    elem = pCTSetELem->firstChildElement("ktd");
    if (!elem.isNull()) {
        m_sSettings.ktd = elem.attribute("val").toDouble();
    }

    // 一次时间常数
    elem = pCTSetELem->firstChildElement("Tp");
    if (!elem.isNull()) {
        m_sSettings.Tp = elem.attribute("val").toDouble();
    }

    // 二次时间常数
    elem = pCTSetELem->firstChildElement("Ts");
    if (!elem.isNull()) {
        m_sSettings.Ts = elem.attribute("val").toDouble();
    }

    // 工作循环
    elem = pCTSetELem->firstChildElement("coc");
    if (!elem.isNull()) {
        m_sSettings.coc = elem.attribute("val").toDouble();
    }

    // t1
    elem = pCTSetELem->firstChildElement("t1");
    if (!elem.isNull()) {
        m_sSettings.t1 = elem.attribute("val").toDouble();
    }

    // tal1
    elem = pCTSetELem->firstChildElement("tal1");
    if (!elem.isNull()) {
        m_sSettings.tal1 = elem.attribute("val").toDouble();
    }

    // tfr
    elem = pCTSetELem->firstChildElement("tfr");
    if (!elem.isNull()) {
        m_sSettings.tfr = elem.attribute("val").toDouble();
    }

    // t2
    elem = pCTSetELem->firstChildElement("t2");
    if (!elem.isNull()) {
        m_sSettings.t2 = elem.attribute("val").toDouble();
    }

    // tal2
    elem = pCTSetELem->firstChildElement("tal2");
    if (!elem.isNull()) {
        m_sSettings.tal2 = elem.attribute("val").toDouble();
    }

    return true;
}

bool CCTExtendParamSettings_TPX::saveParam(QDomDocument* pDoc, QDomElement* pCTSetELem)
{
    Q_ASSERT(pDoc);
    Q_ASSERT(pCTSetELem);

    // 额定对称短路电流系数
    QDomElement elem = pDoc->createElement("kssc");
    elem.setAttribute("val", m_sSettings.kssc);
    pCTSetELem->appendChild(elem);

    // 暂态面积系数
    elem = pDoc->createElement("ktd");
    elem.setAttribute("val", m_sSettings.ktd);
    pCTSetELem->appendChild(elem);

    // 一次时间常数
    elem = pDoc->createElement("Tp");
    elem.setAttribute("val", m_sSettings.Tp);
    pCTSetELem->appendChild(elem);

    // 二次时间常数
    elem = pDoc->createElement("Ts");
    elem.setAttribute("val", m_sSettings.Ts);
    pCTSetELem->appendChild(elem);

    // 工作循环
    elem = pDoc->createElement("coc");
    elem.setAttribute("val", m_sSettings.coc);
    pCTSetELem->appendChild(elem);

    // t1
    elem = pDoc->createElement("t1");
    elem.setAttribute("val", m_sSettings.t1);
    pCTSetELem->appendChild(elem);

    // tal1
    elem = pDoc->createElement("tal1");
    elem.setAttribute("val", m_sSettings.tal1);
    pCTSetELem->appendChild(elem);

    // tfr
    elem = pDoc->createElement("tfr");
    elem.setAttribute("val", m_sSettings.tfr);
    pCTSetELem->appendChild(elem);

    // t2
    elem = pDoc->createElement("t2");
    elem.setAttribute("val", m_sSettings.t2);
    pCTSetELem->appendChild(elem);

    // tal2
    elem = pDoc->createElement("tal2");
    elem.setAttribute("val", m_sSettings.tal2);
    pCTSetELem->appendChild(elem);

    return true;
}

void CCTExtendParamSettings_TPX::defaultSettings()
{
    m_sSettings.kssc = 20.0;                            // 额定对称短路电流系数
    m_sSettings.ktd = 20.0;                             // 暂态面积系数
    m_sSettings.Tp = 100.0;                             // 一次时间常数
    m_sSettings.Ts = 3000.0;                            // 二次时间常数
    m_sSettings.t1 = 100.0;                             // t1
    m_sSettings.tal1 = 40.0;                            // tal1
    m_sSettings.coc = 0;                                // 工作循环
    m_sSettings.tfr = 500.0;                            // tfr
    m_sSettings.t2 = 100.0;                             // t2
    m_sSettings.tal2 = 40.0;                            // tal2
}


/*************************************
 * CCTExtendParamSettings_TPY
 * ***********************************/

CCTExtendParamSettings_TPY::CCTExtendParamSettings_TPY()
{
    defaultSettings();
}

CCTExtendParamSettings_TPY::~CCTExtendParamSettings_TPY()
{

}

bool CCTExtendParamSettings_TPY::loadParam(QDomElement* pCTSetELem)
{
    Q_ASSERT(pCTSetELem);

    // 置为默认值
    defaultSettings();

    // 额定对称短路电流系数
    QDomElement elem = pCTSetELem->firstChildElement("kssc");
    if (!elem.isNull()) {
        m_sSettings.kssc = elem.attribute("val").toDouble();
    }

    // 暂态面积系数
    elem = pCTSetELem->firstChildElement("ktd");
    if (!elem.isNull()) {
        m_sSettings.ktd = elem.attribute("val").toDouble();
    }

    // 一次时间常数
    elem = pCTSetELem->firstChildElement("Tp");
    if (!elem.isNull()) {
        m_sSettings.Tp = elem.attribute("val").toDouble();
    }

    // 二次时间常数
    elem = pCTSetELem->firstChildElement("Ts");
    if (!elem.isNull()) {
        m_sSettings.Ts = elem.attribute("val").toDouble();
    }

    // 工作循环
    elem = pCTSetELem->firstChildElement("coc");
    if (!elem.isNull()) {
        m_sSettings.coc = elem.attribute("val").toDouble();
    }

    // t1
    elem = pCTSetELem->firstChildElement("t1");
    if (!elem.isNull()) {
        m_sSettings.t1 = elem.attribute("val").toDouble();
    }

    // tal1
    elem = pCTSetELem->firstChildElement("tal1");
    if (!elem.isNull()) {
        m_sSettings.tal1 = elem.attribute("val").toDouble();
    }

    // tfr
    elem = pCTSetELem->firstChildElement("tfr");
    if (!elem.isNull()) {
        m_sSettings.tfr = elem.attribute("val").toDouble();
    }

    // t2
    elem = pCTSetELem->firstChildElement("t2");
    if (!elem.isNull()) {
        m_sSettings.t2 = elem.attribute("val").toDouble();
    }

    // tal2
    elem = pCTSetELem->firstChildElement("tal2");
    if (!elem.isNull()) {
        m_sSettings.tal2 = elem.attribute("val").toDouble();
    }

    return true;
}

bool CCTExtendParamSettings_TPY::saveParam(QDomDocument* pDoc, QDomElement* pCTSetELem)
{
    Q_ASSERT(pDoc);
    Q_ASSERT(pCTSetELem);

    // 额定对称短路电流系数
    QDomElement elem = pDoc->createElement("kssc");
    elem.setAttribute("val", m_sSettings.kssc);
    pCTSetELem->appendChild(elem);

    // 暂态面积系数
    elem = pDoc->createElement("ktd");
    elem.setAttribute("val", m_sSettings.ktd);
    pCTSetELem->appendChild(elem);

    // 一次时间常数
    elem = pDoc->createElement("Tp");
    elem.setAttribute("val", m_sSettings.Tp);
    pCTSetELem->appendChild(elem);

    // 二次时间常数
    elem = pDoc->createElement("Ts");
    elem.setAttribute("val", m_sSettings.Ts);
    pCTSetELem->appendChild(elem);

    // 工作循环
    elem = pDoc->createElement("coc");
    elem.setAttribute("val", m_sSettings.coc);
    pCTSetELem->appendChild(elem);

    // t1
    elem = pDoc->createElement("t1");
    elem.setAttribute("val", m_sSettings.t1);
    pCTSetELem->appendChild(elem);

    // tal1
    elem = pDoc->createElement("tal1");
    elem.setAttribute("val", m_sSettings.tal1);
    pCTSetELem->appendChild(elem);

    // tfr
    elem = pDoc->createElement("tfr");
    elem.setAttribute("val", m_sSettings.tfr);
    pCTSetELem->appendChild(elem);

    // t2
    elem = pDoc->createElement("t2");
    elem.setAttribute("val", m_sSettings.t2);
    pCTSetELem->appendChild(elem);

    // tal2
    elem = pDoc->createElement("tal2");
    elem.setAttribute("val", m_sSettings.tal2);
    pCTSetELem->appendChild(elem);

    return true;
}

void CCTExtendParamSettings_TPY::defaultSettings()
{
    m_sSettings.kssc = 20.0;                            // 额定对称短路电流系数
    m_sSettings.ktd = 20.0;                             // 暂态面积系数
    m_sSettings.Tp = 100.0;                             // 一次时间常数
    m_sSettings.Ts = 3000.0;                            // 二次时间常数
    m_sSettings.coc = 0;                                // 工作循环
    m_sSettings.t1 = 100.0;                             // t1
    m_sSettings.tal1 = 40.0;                            // tal1
    m_sSettings.tfr = 500.0;                            // tfr
    m_sSettings.t2 = 100.0;                             // t2
    m_sSettings.tal2 = 40.0;                            // tal2
}


/*************************************
 * CCTExtendParamSettings_TPZ
 * ***********************************/

CCTExtendParamSettings_TPZ::CCTExtendParamSettings_TPZ()
{
    defaultSettings();
}

CCTExtendParamSettings_TPZ::~CCTExtendParamSettings_TPZ()
{

}

bool CCTExtendParamSettings_TPZ::loadParam(QDomElement* pCTSetELem)
{
    Q_ASSERT(pCTSetELem);

    // 置为默认值
    defaultSettings();

    // 额定对称短路电流系数
    QDomElement elem = pCTSetELem->firstChildElement("kssc");
    if (!elem.isNull()) {
        m_sSettings.kssc = elem.attribute("val").toDouble();
    }

    // 暂态面积系数
    elem = pCTSetELem->firstChildElement("ktd");
    if (!elem.isNull()) {
        m_sSettings.ktd = elem.attribute("val").toDouble();
    }

    // 一次时间常数
    elem = pCTSetELem->firstChildElement("Tp");
    if (!elem.isNull()) {
        m_sSettings.Tp = elem.attribute("val").toDouble();
    }

    // 二次时间常数
    elem = pCTSetELem->firstChildElement("Ts");
    if (!elem.isNull()) {
        m_sSettings.Ts = elem.attribute("val").toDouble();
    }

    return true;
}

bool CCTExtendParamSettings_TPZ::saveParam(QDomDocument* pDoc, QDomElement* pCTSetELem)
{
    Q_ASSERT(pDoc);
    Q_ASSERT(pCTSetELem);

    // 额定对称短路电流系数
    QDomElement elem = pDoc->createElement("kssc");
    elem.setAttribute("val", m_sSettings.kssc);
    pCTSetELem->appendChild(elem);

    // 暂态面积系数
    elem = pDoc->createElement("ktd");
    elem.setAttribute("val", m_sSettings.ktd);
    pCTSetELem->appendChild(elem);

    // 一次时间常数
    elem = pDoc->createElement("Tp");
    elem.setAttribute("val", m_sSettings.Tp);
    pCTSetELem->appendChild(elem);

    // 二次时间常数
    elem = pDoc->createElement("Ts");
    elem.setAttribute("val", m_sSettings.Ts);
    pCTSetELem->appendChild(elem);

    return true;
}

void CCTExtendParamSettings_TPZ::defaultSettings()
{
    m_sSettings.kssc = 20.0;                             // 额定对称短路电流系数
    m_sSettings.ktd = 20.0;                              // 暂态面积系数
    m_sSettings.Tp = 100.0;                              // 一次时间常数
    m_sSettings.Ts = 3000.0;                             // 二次时间常数
}


/*************************************
 * CCTExtendParamSettings_TPS
 * ***********************************/

CCTExtendParamSettings_TPS::CCTExtendParamSettings_TPS()
{
    defaultSettings();
}

CCTExtendParamSettings_TPS::~CCTExtendParamSettings_TPS()
{

}

bool CCTExtendParamSettings_TPS::loadParam(QDomElement* pCTSetELem)
{
    Q_ASSERT(pCTSetELem);

    // 置为默认值
    defaultSettings();

    // 额定对称短路电流系数
    QDomElement elem = pCTSetELem->firstChildElement("kssc");
    if (!elem.isNull()) {
        m_sSettings.kssc = elem.attribute("val").toDouble();
    }

    // 面积系数Kx
    elem = pCTSetELem->firstChildElement("ktd");
    if (!elem.isNull()) {
        m_sSettings.ktd = elem.attribute("val").toDouble();
    }

    // 额定ual
    elem = pCTSetELem->firstChildElement("Ual");
    if (!elem.isNull()) {
        m_sSettings.Ual = elem.attribute("val").toDouble();
    }

    // Ual对应最大Ia1
    elem = pCTSetELem->firstChildElement("Ial");
    if (!elem.isNull()) {
        m_sSettings.Ial = elem.attribute("val").toDouble();
    }

    return true;
}

bool CCTExtendParamSettings_TPS::saveParam(QDomDocument* pDoc, QDomElement* pCTSetELem)
{
    Q_ASSERT(pDoc);
    Q_ASSERT(pCTSetELem);

    // 额定对称短路电流系数
    QDomElement elem = pDoc->createElement("kssc");
    elem.setAttribute("val", m_sSettings.kssc);
    pCTSetELem->appendChild(elem);

    // 面积系数Kx
    elem = pDoc->createElement("ktd");
    elem.setAttribute("val", m_sSettings.ktd);
    pCTSetELem->appendChild(elem);

    // 额定ual
    elem = pDoc->createElement("Ual");
    elem.setAttribute("val", m_sSettings.Ual);
    pCTSetELem->appendChild(elem);

    // Ual对应最大Ia1
    elem = pDoc->createElement("Ial");
    elem.setAttribute("val", m_sSettings.Ial);
    pCTSetELem->appendChild(elem);

    return true;
}

void CCTExtendParamSettings_TPS::defaultSettings()
{
    m_sSettings.kssc = 20.0;                           // 额定对称短路电流系数
    m_sSettings.ktd = 20.0;                            // 面积系数Kx
    m_sSettings.Ual = 10000.0;                         // 额定ual
    m_sSettings.Ial = 0.5;                             // Ual对应最大Ia1
}
