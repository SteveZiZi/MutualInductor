#ifndef CSYSTEMINFOSETTINGS_H
#define CSYSTEMINFOSETTINGS_H

#include <QString>

struct S_System_Info
{
    QString applicationVersion;
    QString hardwareVersion;
    QString systemVersion;
};

class CSystemInfoSettings
{
public:
    static QString applicationVersion() {return getInstance()->m_sSettings.applicationVersion;}
    static QString hardwareVersion() {return getInstance()->m_sSettings.hardwareVersion;}
    static QString systemVersion() {return getInstance()->m_sSettings.systemVersion;}

private:
    CSystemInfoSettings();

    static CSystemInfoSettings * getInstance();

    void defaultSettings();
    //  从配置文件加载配置
    bool loadSettingsFromFile();

    void getSystemVersion();

private:
    S_System_Info                   m_sSettings;
};

#endif // CSYSTEMINFOSETTINGS_H
