#include "dataManager.h"
#include <QDomDocument>
#include <QFile>
#include <QDir>
#include <QDebug>

#include "src/CTTester/dataManager/settings/ctBaseParamSetttings.h"
#include "src/CTTester/dataManager/settings/ctExtenParamSettings.h"
#include "src/CTTester/dataManager/settings/ptBaseParamSettings.h"
#ifdef WIN32
#include "src/CTTester/dataManager/settings/resultParamSettings.h"
#endif
#include "src/CTTester/dataManager/settings/systemInfoSettings.h"

#include "src/CTTester/dataManager/device/ctpt_def.h"
#include "src/CTTester/dataManager/device/ctpt_api.hpp"
#include "src/CTTester/dataManager/device/status_mgr.h"


CDataManager::CDataManager()
: m_bInited(false)
, m_ctptCtrl(NULL)
, m_statusMgr(NULL)
, m_pCtBaseParamSettings(NULL)
, m_pCtExtendParamSettingsMeasure(NULL)
, m_pCtExtendParamSettingsP(NULL)
, m_pCtExtendParamSettingsPR(NULL)
, m_pCtExtendParamSettingsPX(NULL)
, m_pCtExtendParamSettingsTPX(NULL)
, m_pCtExtendParamSettingsTPY(NULL)
, m_pCtExtendParamSettingsTPZ(NULL)
, m_pCtExtendParamSettingsTPS(NULL)
, m_pPtBaseParamSettings(NULL)
#ifdef WIN32
, m_pResultParamSettings(NULL)
#endif
, m_type(INDUCTOR_CT)
, m_testItem(TEST_EXCITATION)
{
    Init();
}

CDataManager::~CDataManager()
{
    Deinit();
}

CDataManager *CDataManager::getInstance()
{
    static CDataManager dataMgr;
    return &dataMgr;
}

void CDataManager::Init()
{
    if (!m_bInited)
    {
        if (m_ctptCtrl == NULL) {
            m_ctptCtrl = new CTPT::ctpt_ctrl();
        }
        if (m_statusMgr == NULL) {
            m_statusMgr = CTPT::StatusMgrInst();
        }

        if (m_pCtBaseParamSettings == NULL) {
            m_pCtBaseParamSettings = new CCTBaseParamSettings;
        }
        if (m_pCtExtendParamSettingsMeasure == NULL) {
            m_pCtExtendParamSettingsMeasure = new CCTExtendParamSettings_Measure;
        }
        if (m_pCtExtendParamSettingsP == NULL) {
            m_pCtExtendParamSettingsP = new CCTExtendParamSettings_P;
        }
        if (m_pCtExtendParamSettingsPR == NULL) {
            m_pCtExtendParamSettingsPR = new CCTExtendParamSettings_PR;
        }
        if (m_pCtExtendParamSettingsPX == NULL) {
            m_pCtExtendParamSettingsPX = new CCTExtendParamSettings_PX;
        }
        if (m_pCtExtendParamSettingsTPX == NULL) {
            m_pCtExtendParamSettingsTPX = new CCTExtendParamSettings_TPX;
        }
        if (m_pCtExtendParamSettingsTPY == NULL) {
            m_pCtExtendParamSettingsTPY = new CCTExtendParamSettings_TPY;
        }
        if (m_pCtExtendParamSettingsTPZ == NULL) {
            m_pCtExtendParamSettingsTPZ = new CCTExtendParamSettings_TPZ;
        }
        if (m_pCtExtendParamSettingsTPS == NULL) {
            m_pCtExtendParamSettingsTPS = new CCTExtendParamSettings_TPS;
        }

        if (m_pPtBaseParamSettings == NULL) {
            m_pPtBaseParamSettings = new CPTBaseParamSettings;
        }

#ifdef WIN32
        if (m_pResultParamSettings == NULL) {
            m_pResultParamSettings = new CResultParamSettings;
        }
#endif

        // 加载配置
        loadParam(DATAMGR_PATH_CONFIG_FILE);

        m_bInited = true;
    }
}

void CDataManager::Deinit()
{
    if (m_bInited)
    {
        if (m_ctptCtrl) {
            delete m_ctptCtrl;
            m_ctptCtrl = NULL;
        }

        if (m_pCtBaseParamSettings) {
            delete m_pCtBaseParamSettings;
            m_pCtBaseParamSettings = NULL;
        }
        if (m_pCtExtendParamSettingsMeasure) {
            delete m_pCtExtendParamSettingsMeasure;
            m_pCtExtendParamSettingsMeasure = NULL;
        }
        if (m_pCtExtendParamSettingsP) {
            delete m_pCtExtendParamSettingsP;
            m_pCtExtendParamSettingsP = NULL;
        }
        if (m_pCtExtendParamSettingsPR) {
            delete m_pCtExtendParamSettingsPR;
            m_pCtExtendParamSettingsPR = NULL;
        }
        if (m_pCtExtendParamSettingsPX) {
            delete m_pCtExtendParamSettingsPX;
            m_pCtExtendParamSettingsPX = NULL;
        }
        if (m_pCtExtendParamSettingsTPX) {
            delete m_pCtExtendParamSettingsTPX;
            m_pCtExtendParamSettingsTPX = NULL;
        }
        if (m_pCtExtendParamSettingsTPY) {
            delete m_pCtExtendParamSettingsTPY;
            m_pCtExtendParamSettingsTPY = NULL;
        }
        if (m_pCtExtendParamSettingsTPZ) {
            delete m_pCtExtendParamSettingsTPZ;
            m_pCtExtendParamSettingsTPZ = NULL;
        }
        if (m_pCtExtendParamSettingsTPS) {
            delete m_pCtExtendParamSettingsTPS;
            m_pCtExtendParamSettingsTPS = NULL;
        }

        if (m_pPtBaseParamSettings) {
            delete m_pPtBaseParamSettings;
            m_pPtBaseParamSettings = NULL;
        }

#ifdef WIN32
        if (m_pResultParamSettings) {
            delete m_pResultParamSettings;
            m_pResultParamSettings = NULL;
        }
#endif

        m_bInited = false;
    }
}

bool CDataManager::loadParam(const QString& filePath)
{
    // 打开文件
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << QString("*CDataManager::loadParam* open %1 fail!").arg(filePath);
        return false;
    }

    // 加载文件
    QDomDocument doc;
    int          row = 0, col = 0;
    if (!doc.setContent(&file, false, NULL, &row, &col)) {
        file.close();
        qDebug() << QString("*CDataManager::loadParam* load file failed at row %1 and col %2!").arg(row).arg(col);
        return false;
    } else {
        file.close();
        if (doc.isNull()) {
            qDebug() << "*CDataManager::loadParam* document is null!";
            return false;
        }
    }

    // 判断配置文件是否有效
    QDomElement root = doc.firstChildElement();
    if (root.isNull() || 
        root.tagName().compare("config") != 0 || 
        root.attribute("mask").compare("1234ABCD") != 0) {
        qDebug() << "*CDataManager::loadParam* document is invalid!";
        return false;
    }

    // 获取CT配置
    QDomElement ctset = root.firstChildElement("ctset");
    if (!ctset.isNull()){

        // 基本设置
        Q_ASSERT(m_pCtBaseParamSettings);
        m_pCtBaseParamSettings->loadParam(&ctset);

        // 扩展设置
        switch (m_pCtBaseParamSettings->testLevel() )
        {
        case CTPT_P:        m_pCtExtendParamSettingsP->loadParam(&ctset);        break;
        case CTPT_MEASURE:  m_pCtExtendParamSettingsMeasure->loadParam(&ctset);  break;
        case CTPT_PR:       m_pCtExtendParamSettingsPR->loadParam(&ctset);       break;
        case CTPT_PX:       m_pCtExtendParamSettingsPX->loadParam(&ctset);       break;
        case CTPT_TPX:      m_pCtExtendParamSettingsTPX->loadParam(&ctset);      break;
        case CTPT_TPY:      m_pCtExtendParamSettingsTPY->loadParam(&ctset);      break;
        case CTPT_TPZ:      m_pCtExtendParamSettingsTPZ->loadParam(&ctset);      break;
        case CTPT_TPS:      m_pCtExtendParamSettingsTPS->loadParam(&ctset);      break;
        default:            qDebug() << "*CDataManager::loadParam* unknown level!";
        }
    }

    // 获取PT配置
    QDomElement ptset = root.firstChildElement("ptset");
    if (!ptset.isNull()){

        // 基本设置
        Q_ASSERT(m_pPtBaseParamSettings);
        m_pPtBaseParamSettings->loadParam(&ptset);

        // 扩展设置
    }

    return true;
}

bool CDataManager::saveParam(const QString& filePath)
{
    // 只写方式打开，并清空以前的信息
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))  {
        qDebug() << QString("*CDataManager::saveParam* open %1 fail!").arg(filePath);
        return false;
    }

    // 添加根元素
    QDomDocument doc;
    QDomElement  root = doc.createElement("config");
    root.setAttribute("mask", "1234ABCD");
    doc.appendChild(root); 

    // 添加CT配置元素
    QDomElement  ctset = doc.createElement("ctset");
    root.appendChild(ctset);

    // 填充CT设置（基本+扩展）
    Q_ASSERT(m_pCtBaseParamSettings);
    m_pCtBaseParamSettings->saveParam(&doc, &ctset);
    switch (m_pCtBaseParamSettings->testLevel())
    {
    case CTPT_P:        m_pCtExtendParamSettingsP->saveParam(&doc, &ctset);        break;
    case CTPT_MEASURE:  m_pCtExtendParamSettingsMeasure->saveParam(&doc, &ctset);  break;
    case CTPT_PR:       m_pCtExtendParamSettingsPR->saveParam(&doc, &ctset);       break;
    case CTPT_PX:       m_pCtExtendParamSettingsPX->saveParam(&doc, &ctset);       break;
    case CTPT_TPX:      m_pCtExtendParamSettingsTPX->saveParam(&doc, &ctset);      break;
    case CTPT_TPY:      m_pCtExtendParamSettingsTPY->saveParam(&doc, &ctset);      break;
    case CTPT_TPZ:      m_pCtExtendParamSettingsTPZ->saveParam(&doc, &ctset);      break;
    case CTPT_TPS:      m_pCtExtendParamSettingsTPS->saveParam(&doc, &ctset);      break;
    default:            qDebug() << "*CDataManager::saveParam* unknown level!";
    }

    // 添加PT配置元素
    QDomElement  ptset = doc.createElement("ptset");
    root.appendChild(ptset);

    // 填充PT设置（基本+扩展）
    Q_ASSERT(m_pPtBaseParamSettings);
    m_pPtBaseParamSettings->saveParam(&doc, &ptset);

    QTextStream out(&file);
    out.setCodec("UTF-8");
    doc.save(out, 4);
    file.close();

    return true;
}

bool CDataManager::isTetsting()
{
    return m_statusMgr->isTesting();
}


/*
 *  获取测试的错误代码   0：no error
 */
int CDataManager::getErrCode()
{
    return m_statusMgr->get_test_error_code();
}

QString CDataManager::errCodeToString(int errCode)
{
    return QString(m_statusMgr->error_code_to_string(errCode).c_str());
}

//设置试验项目
void CDataManager::setTestItem(int testItem)
{
    QMutexLocker _lock(&m_mutex);
    m_testItem = testItem;
}

bool CDataManager::getResult()
{
    return m_ctptCtrl->get_ctpt_result(m_result)>=0 ? true : false;
}

const S_CTPT_Result &CDataManager::ctPtResult() const
{
    return m_result;
}

double CDataManager::currentCurrent() const
{
    return m_statusMgr->get_load_cur();
}
double CDataManager::currentVolgate() const
{
    return m_statusMgr->get_load_vol();
}

TEST_ITEM_ENUM CDataManager::currentTestItem()
{
    switch(m_statusMgr->get_current_test_item()) {
    case CTPT::ITEM_RESISTANCE:
        return TEST_RESISTANCE;
        break;
    case CTPT::ITEM_EXCITATION:
        return TEST_EXCITATION;
        break;
    case CTPT::ITEM_RATIO:
        return TEST_RATIO;
        break;
    case CTPT::ITEM_BURDEN:
        return TEST_BURDEN;
        break;
    case CTPT::ITEM_DEMAGNETIOZATION:
        return TEST_DEMAGNETIOZATION;
        break;
    default:
        break;
    }

    return TEST_NONE;
}

CTPT::TEST_STATUS_ENUM CDataManager::testStatus()
{
    return m_statusMgr->get_test_status();
}

QString CDataManager::arithmeticVersion()
{
    return QString(m_ctptCtrl->ctpt_ver().c_str());
}
QString CDataManager::fpgaVersion()
{
    return QString(m_ctptCtrl->fpga_ver().c_str());
}
QString CDataManager::applicationVersion()
{
    return CSystemInfoSettings::applicationVersion();
}
QString CDataManager::hardwareVersion()
{
    return CSystemInfoSettings::hardwareVersion();
}
QString CDataManager::systemVersion()
{
    return CSystemInfoSettings::systemVersion();
}

void CDataManager::startTest()
{
    if (!m_statusMgr->isTesting()) {
        startTester();
        emit testStarted();
    }
}

void CDataManager::stopTest()
{
    stopTester();
    emit testStopped();
}

bool CDataManager::startTester()
{
    if(INDUCTOR_CT == m_type)
        setCtParam();
    else if(INDUCTOR_PT == m_type)
        setPtParam();

    return m_ctptCtrl->start();
}

bool CDataManager::stopTester()
{
    return m_ctptCtrl->stop();
}

// 设置CT类型互感器的参数
void CDataManager::setCtParam()
{
    QMutexLocker _lock(&m_mutex);
    CTPT::ct_setting settings;

    const S_CT_Base_Param &baseParam = m_pCtBaseParamSettings->param();
    settings.baseParam.testItem = m_testItem;
    settings.baseParam.testCurrent = baseParam.testCurrent;
    settings.baseParam.primaryCurrent = baseParam.primaryCurrent;
    settings.baseParam.secondCurrent = baseParam.secondCurrent;
    settings.baseParam.rated_frequency = 50.0f;
    settings.baseParam.ratedLoad = baseParam.ratedLoad;
    settings.baseParam.powerFactor = baseParam.powerFactor;
    settings.baseParam.currentTemp = baseParam.currentTemp;
    settings.baseParam.destTemp = baseParam.destTemp;

    switch(baseParam.level)
    {
    case CTPT_P:
    {
        settings.baseParam.level = CTPT::CTC_P;

        const S_CT_P_Extend_Param &extendParam = m_pCtExtendParamSettingsP->param();
        settings.extendParam.p.alf = extendParam.alf;                                   // 额定仪表保安系数
        break;
    }
    case CTPT_MEASURE:
    {
        settings.baseParam.level = CTPT::CTC_MEASURE;
        const S_CT_Measure_Extend_Param &extendParam = m_pCtExtendParamSettingsMeasure->param();
        settings.extendParam.measure.fs = extendParam.fs;                               // 额定仪表保安系数
        settings.extendParam.measure.extendCurrent = extendParam.extendCurrent;         // 扩展电流标定
        break;
    }
    case CTPT_PR:
    {
        settings.baseParam.level = CTPT::CTC_PR;
        const S_CT_PR_Extend_Param &extendParam = m_pCtExtendParamSettingsPR->param();
        settings.extendParam.pr.alf = extendParam.alf;                                  // 额定仪表保安系数
        settings.extendParam.pr.Ts  = extendParam.Ts;                                   // 二次时间常数
        break;
    }
    case CTPT_PX:
    {
        settings.baseParam.level = CTPT::CTC_PX;
        const S_CT_PX_Extend_Param &extendParam = m_pCtExtendParamSettingsPX->param();
        settings.extendParam.px.Kx = extendParam.Kx;                                    // 额定计算系数Kx
        settings.extendParam.px.Ek  = extendParam.Ek;                                   // 额定拐点电势
        settings.extendParam.px.Ie  = extendParam.Ie;                                   // Ek对应Ie
        break;
    }
    case CTPT_TPX:
    {
        settings.baseParam.level = CTPT::CTC_TPX;
        const S_CT_TPX_Extend_Param &extendParam = m_pCtExtendParamSettingsTPX->param();
        settings.extendParam.tpx.kssc = extendParam.kssc;
        settings.extendParam.tpx.ktd = extendParam.ktd;
        settings.extendParam.tpx.Tp = extendParam.Tp;
        settings.extendParam.tpx.Ts = extendParam.Ts;
        settings.extendParam.tpx.t1 = extendParam.t1;
        settings.extendParam.tpx.tal1 = extendParam.tal1;
        settings.extendParam.tpx.coc = extendParam.coc;
        settings.extendParam.tpx.tfr = extendParam.tfr;
        settings.extendParam.tpx.t2 = extendParam.t2;
        settings.extendParam.tpx.tal2 = extendParam.tal2;
        break;
    }
    case CTPT_TPY:
    {
        settings.baseParam.level = CTPT::CTC_TPY;
        const S_CT_TPY_Extend_Param &extendParam = m_pCtExtendParamSettingsTPY->param();
        settings.extendParam.tpy.kssc = extendParam.kssc;
        settings.extendParam.tpy.ktd = extendParam.ktd;
        settings.extendParam.tpy.Tp = extendParam.Tp;
        settings.extendParam.tpy.Ts = extendParam.Ts;
        settings.extendParam.tpy.t1 = extendParam.t1;
        settings.extendParam.tpy.tal1 = extendParam.tal1;
        settings.extendParam.tpy.coc = extendParam.coc;
        settings.extendParam.tpy.tfr = extendParam.tfr;
        settings.extendParam.tpy.t2 = extendParam.t2;
        settings.extendParam.tpy.tal2 = extendParam.tal2;
        break;
    }
    case CTPT_TPZ:
    {
        settings.baseParam.level = CTPT::CTC_TPZ;
        const S_CT_TPZ_Extend_Param &extendParam = m_pCtExtendParamSettingsTPZ->param();
        settings.extendParam.tpz.kssc = extendParam.kssc;
        settings.extendParam.tpz.ktd = extendParam.ktd;
        settings.extendParam.tpz.Tp = extendParam.Tp;
        settings.extendParam.tpz.Ts = extendParam.Ts;
        break;
    }
    case CTPT_TPS:
    {
        settings.baseParam.level = CTPT::CTC_TPS;
        const S_CT_TPS_Extend_Param &extendParam = m_pCtExtendParamSettingsTPS->param();
        settings.extendParam.tps.kssc = extendParam.kssc;
        settings.extendParam.tps.ktd = extendParam.ktd;
        settings.extendParam.tps.Ual = extendParam.Ual;
        settings.extendParam.tps.Ial = extendParam.Ial;
        break;
    }
    default:
        break;
    }

    m_ctptCtrl->config(settings);
}

// 设置PT类型互感器的参数
void CDataManager::setPtParam()
{
    QMutexLocker _lock(&m_mutex);
    CTPT::pt_setting settings;

    const S_PT_Base_Param &baseParam = m_pPtBaseParamSettings->param();

    settings.testItem = m_testItem;

    settings.testCurrent = baseParam.testCur;
    settings.testVoltage = baseParam.testVol;

    settings.rated_frequency = 50.0f;

    settings.primaryVol = baseParam.primaryVol;
    settings.secondVol = baseParam.secondVol;

    settings.currentTemp = baseParam.currentTemp;
    settings.destTemp = baseParam.destTemp;

    switch(baseParam.level) {
    case CTPT_MEASURE:
        settings.level = CTPT::CTC_MEASURE;
        break;
    default:
        settings.level = CTPT::CTC_P;
        break;
    }
    m_ctptCtrl->config(settings);
}
