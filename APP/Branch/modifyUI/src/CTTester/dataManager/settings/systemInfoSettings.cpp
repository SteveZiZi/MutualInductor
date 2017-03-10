#include "systemInfoSettings.h"

#include <QFile>
#include <QDomDocument>
#include <QDir>
#include <QDebug>


#define SYSTEM_INFO_SETTINGS_FILE    ".\\config\\systemInfoSettings.xml"
#define SYSTEM_VERSION_FILE          "/etc/ver.conf"

CSystemInfoSettings::CSystemInfoSettings()
{
    if(!loadSettingsFromFile())
    {
        defaultSettings();
    }
}

CSystemInfoSettings *CSystemInfoSettings::getInstance()
{
    static CSystemInfoSettings systemInfoSettings;
    return &systemInfoSettings;
}

void CSystemInfoSettings::defaultSettings()
{
    m_sSettings.applicationVersion = "1.0.1.341";
    m_sSettings.hardwareVersion = "1.0.3";
    getSystemVersion();
}

bool CSystemInfoSettings::loadSettingsFromFile()
{
    QFile file(SYSTEM_INFO_SETTINGS_FILE);
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        QString string(QString("systemInfoSettings::load open %1 fail.").arg(SYSTEM_INFO_SETTINGS_FILE));
        qDebug() << string;
        return false;
    }

    QDomDocument document;
    QString error;
    int row = 0, column = 0;
    if(!document.setContent(&file, false, &error, &row, &column)) {
        qDebug() << QString("load parse file failed at line row and column %1,%2").arg(row).arg(column);
        return false;
    }
    file.close();

    if(document.isNull()) {
        qDebug() << "systemInfoSettings::load document is null!";
        return false;
    }

    QDomElement root = document.documentElement();
    if (root.isNull()) {
        return false;
    }

    m_sSettings.applicationVersion = root.firstChildElement(QString("applicationVersion")).attribute(QString("val"));
    m_sSettings.hardwareVersion = root.firstChildElement(QString("hardwareVersion")).attribute(QString("val"));

    return true;
}

void CSystemInfoSettings::getSystemVersion()
{
#ifndef WIN32
    QFile file(SYSTEM_VERSION_FILE);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "open brightness fail";
        return;
    }
    QTextStream in(&file);
    QStringList stringList = in.readAll().split("\n");
    QString version = stringList[0];
    QStringList versionList = version.split(":");
    m_sSettings.systemVersion = versionList[1];
    file.close();
#else
    m_sSettings.systemVersion = "1.0.7";
#endif
}

