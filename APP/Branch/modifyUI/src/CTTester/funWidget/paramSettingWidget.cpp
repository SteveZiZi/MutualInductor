#include "paramSettingWidget.h"

#include <QAction>
#include <QDebug>
#include <QRadioButton>
#include <QLineEdit>
#include <QPalette>
#include <QPainter>
#include <QKeyEvent>
#include <QApplication>

#include "src/CTTester/common.h"
#include "src/CTTester/funWidget/paramSetting/testItemWidget.h"
#include "src/CTTester/funWidget/paramSetting/mutualInductorTypeWidget.h"
#include "src/CTTester/funWidget/paramSetting/baseParamSettingsWidget.h"
#include "src/CTTester/funWidget/paramSetting/extendParamWidget.h"
#include "src/CTTester/funWidget/paramSetting/bottomStatusWidget.h"
#include "src/CTTester/overrideControl/mqmessagebox.h"
#include "src/CTTester/dialog/QFileDlg.h"

#include "src/CTTester/dataManager/dataManager.h"


CParamSettingWidget::CParamSettingWidget(QWidget *parent)
    : CBasePageWidget(parent)
    , m_bMessageBoxPoped(false)
    , m_bManualStoped(false)
{
    initWidget();
    initMenu();
    localConnection();
    addEventFilter();
}

void CParamSettingWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_F6:
        onStartTest();
        event->accept();
        break;
    case Qt::Key_F7:
        onStopTest();
        event->accept();
        break;
    default:
        event->ignore();
        break;
    }
}

bool CParamSettingWidget::isManualStoped() const
{
    return m_bManualStoped;
}

int CParamSettingWidget::btnsCount() const
{
    return this->actions().size();
}

void CParamSettingWidget::focusInEvent(QFocusEvent *event)
{
    focusNextChild();

    return QWidget::focusInEvent(event);
}

bool CParamSettingWidget::eventFilter(QObject *obj, QEvent *eve)
{
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(eve);
    if(m_pMutualInductorTypeWidget == obj)
    {
        if(QEvent::KeyPress == keyEvent->type())
        {
            switch (keyEvent->key())
            {
            case Qt::Key_Up:{
                m_pMutualInductorTypeWidget->clearFocus();
                QKeyEvent *e = new QKeyEvent(keyEvent->type(),Qt::Key_Escape,Qt::NoModifier);
                QApplication::postEvent(this->parent()->parent(), e);
                return false;
            }
            case Qt::Key_Down:
                m_pMutualInductorTypeWidget->clearFocus();
                m_pBaseParamSettingsWidget->setFocus();
                return true;
            default:
                break;
            }
        }
    }
    else if(m_pTestItemWidget == obj)
    {
        if(QEvent::KeyPress == keyEvent->type())
        {
            switch (keyEvent->key())
            {
            case Qt::Key_Up:{
                m_pTestItemWidget->clearFocus();
                QKeyEvent *e = new QKeyEvent(keyEvent->type(),Qt::Key_Escape,Qt::NoModifier);
                QApplication::postEvent(this->parent()->parent(), e);
                return false;
            }
            case Qt::Key_Down:
                m_pTestItemWidget->clearFocus();
                m_pBaseParamSettingsWidget->setFocus();
                return true;
            case Qt::Key_Right:{
                m_pTestItemWidget->clearFocus();
                QKeyEvent *e = new QKeyEvent(keyEvent->type(),keyEvent->key(),Qt::NoModifier);
                QApplication::postEvent(this->parent()->parent(), e);
                return false;
            }
            default:
                break;
            }
        }
    }
    else if(m_pBaseParamSettingsWidget == obj)
    {
        if(QEvent::KeyPress == keyEvent->type())
        {
            switch (keyEvent->key())
            {
            case Qt::Key_Up:
                m_pBaseParamSettingsWidget->clearFocus();
                m_pTestItemWidget->setFocus();
                return true;
            case Qt::Key_Right:
                m_pBaseParamSettingsWidget->clearFocus();
                m_pExtendParamWidget->setFocus();
                return true;
            default:
                break;
            }
        }
    }
    else if(m_pExtendParamWidget == obj)
    {
        if(QEvent::KeyPress == keyEvent->type())
        {
            switch (keyEvent->key())
            {
            case Qt::Key_Up:
                m_pExtendParamWidget->clearFocus();
                m_pTestItemWidget->setFocus();
                return true;
            case Qt::Key_Left:
                m_pExtendParamWidget->clearFocus();
                m_pBaseParamSettingsWidget->setFocus();
                return true;
            case Qt::Key_Right:{
                m_pExtendParamWidget->clearFocus();
                QKeyEvent *e = new QKeyEvent(keyEvent->type(),keyEvent->key(),Qt::NoModifier);
                QApplication::postEvent(this->parent()->parent(), e);
                return false;
            }
            default:
                break;
            }
        }
    }

    return false;
}

void CParamSettingWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    int x0 = BASE_PARAM_WIDGET_L;
    int y0 = BASE_PARAM_WIDGET_T-10;
    int x1 = x0+FUNC_PAGE_W-20;
    int y1 = y0;

    painter.drawLine(x0, y0, x1, y1);
}

void CParamSettingWidget::onStartTest()
{
    if(m_bMessageBoxPoped || CDataManager::getInstance()->isTetsting())
        return;

    m_bMessageBoxPoped = true;

    // 用HTML中的pre标签支持换行 eg:<pre><font color=red>请确认接线正确!\r\n \r\n是否开始进行测试?</font></pre>
    if(popSwitchCtrlDlg(tr("开始测试?"), tr("请确认接线正确!\r\n \r\n是否开始进行测试?")) == QMessageBox::Cancel)
    {
        m_bMessageBoxPoped = false;
        return;
    }
    else
    {
        CDataManager::getInstance()->startTest();
        setControlsEnable(false);

//        m_pStopButton->setEnabled(true);
//        m_pStopButton->setFocus();

        emit enableFunTab(false);
        emit startTest();

        m_bMessageBoxPoped = false;
        m_bManualStoped = false;
    }
}

void CParamSettingWidget::onStopTest()
{
    if(CDataManager::getInstance()->isTetsting())
    {
        CDataManager::getInstance()->stopTest();
        emit stopTest();
        m_bManualStoped = true;
    }
}

void CParamSettingWidget::stopTestAuto()
{
    CDataManager::getInstance()->stopTest();
    emit enableFunTab(true);
    setControlsEnable(true);
}

void CParamSettingWidget::onDisplayResult()
{
    emit displayResult();
}

void CParamSettingWidget::onLoadParam()
{
    QDir temp;
    if (!temp.exists(DATAMGR_DIR_CONFIG_FILE_BACKUP)) {
        temp.mkdir(DATAMGR_DIR_CONFIG_FILE_BACKUP);
    }

    QFileDlg *dlg = new QFileDlg();
    dlg->setRootPath(DATAMGR_PATH_ROOT);
    dlg->setCurrentPath(DATAMGR_DIR_CONFIG_FILE_BACKUP);
    dlg->setNameFilters(QStringList() << "*.xml");
    if (dlg->exec() == QDialog::Accepted) {
        QString path = dlg->getFilePath();
        if (!CDataManager::getInstance()->loadParam(path)) {
            QMessageBox::information(NULL, tr("提示"), tr("加载配置失败"));
        }
    }

    emit reloadParam(m_pMutualInductorTypeWidget->getInductorType());
}

void CParamSettingWidget::onSaveParam()
{
    QDir temp;
    if (!temp.exists(DATAMGR_DIR_CONFIG_FILE_BACKUP)) {
        temp.mkdir(DATAMGR_DIR_CONFIG_FILE_BACKUP);
    }

    QFileDlg *dlg = new QFileDlg();
    dlg->setRootPath(DATAMGR_PATH_ROOT);
    dlg->setCurrentPath(DATAMGR_DIR_CONFIG_FILE_BACKUP);
    dlg->setNameFilters(QStringList() << "*.xml");
    if (dlg->exec() == QDialog::Accepted) {
        QString path = dlg->getFilePath();
        if (!CDataManager::getInstance()->saveParam(path)) {
            QMessageBox::information(NULL, tr("提示"), tr("保存配置失败"));
        }
    }
}

void CParamSettingWidget::initMenu()
{
    this->addAction(new QAction(tr("开始试验"), this));
    this->addAction(new QAction(tr("停止试验"), this));
    this->addAction(new QAction(tr("导入参数"), this));
    this->addAction(new QAction(tr("导出参数"), this));

    QList<QAction*> actions = this->actions();
    connect(actions[0], SIGNAL(triggered()), this, SLOT(onStartTest()));
    connect(actions[1], SIGNAL(triggered()), this, SLOT(onStopTest()));
    connect(actions[2], SIGNAL(triggered()), this, SLOT(onLoadParam()));
    connect(actions[3], SIGNAL(triggered()), this, SLOT(onSaveParam()));
}

void CParamSettingWidget::initWidget()
{
    m_pMutualInductorTypeWidget = new CMutualInductorTypeWidget(this);
    m_pMutualInductorTypeWidget->setGeometry(MUTUAL_TYPE_SHOW_L,MUTUAL_TYPE_SHOW_T,
                                             MUTUAL_TYPE_SHOW_W,MUTUAL_TYPE_SHOW_H);

    m_pTestItemWidget = new CTestItemWidget(this);
    m_pTestItemWidget->setGeometry(ITEM_TYPE_SHOW_L,ITEM_TYPE_SHOW_T,
                                   ITEM_TYPE_SHOW_W,ITEM_TYPE_SHOW_H);

    m_pBaseParamSettingsWidget = new CBaseParamSettingsWidget(this);
    m_pBaseParamSettingsWidget->setGeometry(BASE_PARAM_WIDGET_L,BASE_PARAM_WIDGET_T,
                                           BASE_PARAM_WIDGET_W,BASE_PARAM_WIDGET_H);


    m_pExtendParamWidget = new CExtendParamWidget(this);
    m_pExtendParamWidget->setGeometry(EXTEND_PARAM_WIDGET_L,EXTEND_PARAM_WIDGET_T,
                                      EXTEND_PARAM_WIDGET_W,EXTEND_PARAM_WIDGET_H);

    m_pBottomStatusWidget = new CBottomStatusWidget(this);
    m_pBottomStatusWidget->setGeometry(BOTTOM_BAR_L,BOTTOM_BAR_T,
                                       BOTTOM_BAR_W,BOTTOM_BAR_H);
}

void CParamSettingWidget::localConnection()
{
    // 互感器类型选择
    connect(m_pMutualInductorTypeWidget,&CMutualInductorTypeWidget::inductorChanged,
            m_pTestItemWidget,&CTestItemWidget::inductorChang);

    connect(m_pMutualInductorTypeWidget,&CMutualInductorTypeWidget::inductorChanged,
            m_pBaseParamSettingsWidget,&CBaseParamSettingsWidget::inductorChang);

    connect(m_pMutualInductorTypeWidget,&CMutualInductorTypeWidget::inductorChanged,
            m_pExtendParamWidget,&CExtendParamWidget::inductorChang);

    connect(this, &CParamSettingWidget::reloadParam,
        m_pBaseParamSettingsWidget, &CBaseParamSettingsWidget::inductorChang);

    connect(this, &CParamSettingWidget::reloadParam,
        m_pExtendParamWidget, &CExtendParamWidget::inductorChang);

    // 实验项目选择
    // Qt5的信号和槽带参数的使用方法之一————类函数指针
    //void (MQCheckBox::* )(bool) = &MQCheckBox::clicked;
    //void (CExtendParamWidget::* )(bool) = &CExtendParamWidget::onChangeChecked;
    //connect(m_pTestItemWidget->m_pChangeRateCheck,&MQCheckBox::clicked,
    //        m_pExtendParamWidget,&CExtendParamWidget::onChangeChecked);

    // 基本参数选择
    // Qt5的信号和槽带参数的使用方法之二————进行动态转换
    //connect(m_pBaseParamWidget->m_pLevelComBox,static_cast<void (MQComboBox::*)(int)>(&MQComboBox::currentIndexChanged),
    //        m_pExtendParamWidget,static_cast<void (CExtendParamWidget::*)(int)>(&CExtendParamWidget::onLevelChanged));

    // 基本参数选择
    connect(m_pBaseParamSettingsWidget, static_cast<void (CBaseParamSettingsWidget::*)(int)>(&CBaseParamSettingsWidget::inductorClassChanged),
            m_pExtendParamWidget, static_cast<void (CExtendParamWidget::*)(int)>(&CExtendParamWidget::onLevelChanged));

    // 实验
    connect(this,&CParamSettingWidget::startTest,
            m_pBottomStatusWidget,&CBottomStatusWidget::onStartTest);

    connect(this,&CParamSettingWidget::stopTest,
            m_pBottomStatusWidget,&CBottomStatusWidget::onStopTest);

    // 实验结果
    connect(m_pBottomStatusWidget,&CBottomStatusWidget::stopTest,
            this,&CParamSettingWidget::stopTestAuto);

    connect(m_pBottomStatusWidget,&CBottomStatusWidget::displayResult,
            this,&CParamSettingWidget::onDisplayResult);
}

void CParamSettingWidget::addEventFilter()
{
    m_pMutualInductorTypeWidget->installEventFilter(this);
    m_pTestItemWidget->installEventFilter(this);
    m_pBaseParamSettingsWidget->installEventFilter(this);
    m_pExtendParamWidget->installEventFilter(this);
    m_pBottomStatusWidget->installEventFilter(this);
}

int CParamSettingWidget::popSwitchCtrlDlg(const QString &title, const QString &msg)
{
    MQMessageBox messagebox(QMessageBox::Warning,title,msg,QMessageBox::Ok|QMessageBox::Cancel);
    messagebox.setIconPixmap(QPixmap(":/resource/images/warn.png"));
    messagebox.setButtonText(QMessageBox::Ok,tr("确定"));
    messagebox.button(QMessageBox::Ok)->setObjectName("ok_messagebox");
    messagebox.setButtonText(QMessageBox::Cancel,tr("取消"));
    messagebox.button(QMessageBox::Cancel)->setObjectName("cancel_messagebox");
    messagebox.setWindowModality(Qt::WindowModal);
    return messagebox.exec();
}

void CParamSettingWidget::setControlsEnable(bool flag)
{
    m_pMutualInductorTypeWidget->setEnabled(flag);
    m_pTestItemWidget->setEnabled(flag);
    m_pBaseParamSettingsWidget->setEnabled(flag);
    m_pExtendParamWidget->setEnabled(flag);
}

