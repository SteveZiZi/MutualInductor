#ifndef CDATAMANAGER_H
#define CDATAMANAGER_H

#include <QObject>
#include <QtGlobal>
#include <QMutex>

#include "src/CTTester/dataManager/device/paramdef.h"
#include "src/CTTester/dataManager/device/status_mgr.h"

// 根目录的默认路径
#define  DATAMGR_PATH_ROOT                  "."

// 配置文件的默认路径
#define  DATAMGR_PATH_CONFIG_FILE           "./config/config.xml"

// 配置备份的默认路径
#define  DATAMGR_DIR_CONFIG_FILE_BACKUP     "./config_backup"

namespace CTPT {
    class ctpt_ctrl;
    class status_mgr;
}

class CCTBaseParamSettings;
class CCTExtendParamSettings_Measure;
class CCTExtendParamSettings_P;
class CCTExtendParamSettings_PR;
class CCTExtendParamSettings_PX;
class CCTExtendParamSettings_TPX;
class CCTExtendParamSettings_TPY;
class CCTExtendParamSettings_TPZ;
class CCTExtendParamSettings_TPS;
class CPTBaseParamSettings;
class CResultParamSettings;

class CDataManager : public QObject
{
    Q_OBJECT
public:
    static CDataManager* getInstance();

    // 初始化
    void Init();
    void Deinit();

    // 加载配置
    bool loadParam(const QString& filePath);

    // 保存配置
    bool saveParam(const QString& filePath);

    /*
     *  互感器类型
     */
    INDUCTOR_TYPE_ENUM getInductorType() {QMutexLocker _lock(&m_mutex); return m_type;}
    void setInductorType(INDUCTOR_TYPE_ENUM type) {QMutexLocker _lock(&m_mutex); m_type = type;}

    //设置试验项目
    void setTestItem(int testItem);
    int  getTestItem() {QMutexLocker _lock(&m_mutex); return m_testItem;}

    // 获取测试结果
    bool getResult();

    // 返回测试结果
    const S_CTPT_Result &ctPtResult() const;

    /**********************************************************************************
     * 以下为监控所需要的数据
     * ********************************************************************************/
    bool isTetsting();
    double currentCurrent() const;
    double currentVolgate() const;
    /*
     *  Test Item
     */
    TEST_ITEM_ENUM currentTestItem();
    /*
     * Test Status
     */
    CTPT::TEST_STATUS_ENUM testStatus();
    /*
     *  获取测试的错误代码   0：no error
     */
    int getErrCode();
    QString errCodeToString(int errCode);



    // 获取每种设置参数指针
    CCTBaseParamSettings*            ctBaseParamSettings() const {return m_pCtBaseParamSettings;}
    CCTExtendParamSettings_Measure*  ctExtendParamMeasureSettings() const {return m_pCtExtendParamSettingsMeasure;}
    CCTExtendParamSettings_P*        ctExtendParamPSettings() const {return m_pCtExtendParamSettingsP;}
    CCTExtendParamSettings_PR*       ctExtendParamPrSettings() const {return m_pCtExtendParamSettingsPR;}
    CCTExtendParamSettings_PX*       ctExtendParamPxSettings() const {return m_pCtExtendParamSettingsPX;}
    CCTExtendParamSettings_TPX*      ctExtendParamTpxSettings() const {return m_pCtExtendParamSettingsTPX;}
    CCTExtendParamSettings_TPY*      ctExtendParamTpySettings() const {return m_pCtExtendParamSettingsTPY;}
    CCTExtendParamSettings_TPZ*      ctExtendParamTpzSettings() const {return m_pCtExtendParamSettingsTPZ;}
    CCTExtendParamSettings_TPS*      ctExtendParamTpsSettings() const {return m_pCtExtendParamSettingsTPS;}
    CPTBaseParamSettings*            ptBaseParamSettings() const {return m_pPtBaseParamSettings;}


    QString arithmeticVersion();        // 算法版本号
    QString fpgaVersion();              // FPGA版本号
    QString applicationVersion();       // 应用版本号
    QString hardwareVersion();          // 硬件版本号
    QString systemVersion();            // 系统版本号

signals:
    void testStarted();                         // 启动测试后会发出该信号
    void testStopped();                         // 停止测试后会发出该信号
    void testError(int errCode);                // 测试过程中监测到异常 发出该信号

public slots:
    void startTest();
    void stopTest();

private:
    CDataManager();
    ~CDataManager();

    bool startTester();                 // 开始测试
    bool stopTester();                  // 停止测试

    void setCtParam();                  // 设置CT类型互感器的参数
    void setPtParam();                  // 设置PT类型互感器的参数

private:
    bool                                    m_bInited;                         // 是否已初始化
    CTPT::ctpt_ctrl                        *m_ctptCtrl;
    CTPT::status_mgr                       *m_statusMgr;
    // CT参数部分
    CCTBaseParamSettings                   *m_pCtBaseParamSettings;
    CCTExtendParamSettings_Measure         *m_pCtExtendParamSettingsMeasure;
    CCTExtendParamSettings_P               *m_pCtExtendParamSettingsP;
    CCTExtendParamSettings_PR              *m_pCtExtendParamSettingsPR;
    CCTExtendParamSettings_PX              *m_pCtExtendParamSettingsPX;
    CCTExtendParamSettings_TPX             *m_pCtExtendParamSettingsTPX;
    CCTExtendParamSettings_TPY             *m_pCtExtendParamSettingsTPY;
    CCTExtendParamSettings_TPZ             *m_pCtExtendParamSettingsTPZ;
    CCTExtendParamSettings_TPS             *m_pCtExtendParamSettingsTPS;

    // PT参数
    CPTBaseParamSettings*                   m_pPtBaseParamSettings;

#ifdef WIN32
    // 结果数据
    CResultParamSettings*                   m_pResultParamSettings;
#endif

    INDUCTOR_TYPE_ENUM                      m_type;             // 互感器类型
    int                                     m_testItem;         // 试验项目

    QMutex                                  m_mutex;

    S_CTPT_Result                           m_result;
};

#endif // CDATAMANAGER_H
