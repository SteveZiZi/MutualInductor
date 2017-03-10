#include "ptExtenParamSettings.h"
#include <QDomDocument>
#include <QFile>
#include <QDomDocument>
#include <QDir>
#include <QDebug>


CPtExtendParamSettings::CPtExtendParamSettings()
{
    defaultSettings();
}

CPtExtendParamSettings::~CPtExtendParamSettings()
{

}

bool CPtExtendParamSettings::loadParam(QDomElement* pPTSetELem)
{
    Q_ASSERT(pPTSetELem);
    return true;
}

bool CPtExtendParamSettings::saveParam(QDomDocument* pDoc, QDomElement* pPTSetELem)
{
    Q_ASSERT(pDoc);
    Q_ASSERT(pPTSetELem);
    return true;
}

void CPtExtendParamSettings::defaultSettings()
{
    //m_sSettings.rated1Voltage = 2000.0;     // 额定一次电压
}
