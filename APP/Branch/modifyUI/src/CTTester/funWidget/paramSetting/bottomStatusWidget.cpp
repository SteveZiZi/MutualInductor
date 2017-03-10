#include "bottomStatusWidget.h"

#include <QLabel>
#include <QTimer>
#include <QDebug>
#include <QPushButton>

#include "src/CTTester/dataManager/dataManager.h"
#include "src/CTTester/overrideControl/mqmessagebox.h"


#define UPDATE_STATUS_TIME_MS  500
#define LABEL_WIDTH            700


CBottomStatusWidget::CBottomStatusWidget(QWidget *parent)
    : QWidget(parent)
    , m_dataMgr(CDataManager::getInstance())
    , m_bStopTest(false)
{
    initWidget();
    localConnect();
}

void CBottomStatusWidget::onStartTest()
{
    m_pUpdateStatusTimer->start(UPDATE_STATUS_TIME_MS);
    onUpdateStatus();
    m_bStopTest = false;
}

void CBottomStatusWidget::onStopTest()
{
    m_pStatusLabel->setText("正在停止...");
    m_bStopTest = true;
}

void CBottomStatusWidget::focusInEvent(QFocusEvent *event)
{
    focusNextChild();

    return QWidget::focusInEvent(event);
}

void CBottomStatusWidget::onUpdateStatus()
{
    if(!m_dataMgr->isTetsting())
    {
        m_pStatusLabel->setText(tr("就绪"));
        m_pUpdateStatusTimer->stop();
        if(CTPT::TEST_STOP_FOR_EXCEPTION == m_dataMgr->testStatus())  {
            popSwitchCtrlDlg(tr("测试异常"), tr("<font color=red>出现异常，停止测试！</font>"));
            emit stopTest();
        }
        else {
            emit stopTest();
            emit displayResult();
        }
    }
    else if (!m_bStopTest)
    {
        QString text;
        switch(m_dataMgr->currentTestItem()) {
        case TEST_NONE:
            text = tr("测试中,当前电压:%1V,当前电流:%2A")
                       .arg(QString::number(m_dataMgr->currentVolgate(),'f',4))
                       .arg(QString::number(m_dataMgr->currentCurrent(),'f',4))
                       /*.arg(QString::number(m_dataMgr->currentPercent()))*/;
            break;
        case TEST_RESISTANCE:
            text = tr("直阻测试中,当前电压:%1V,当前电流:%2A")
                       .arg(QString::number(m_dataMgr->currentVolgate(),'f',4))
                       .arg(QString::number(m_dataMgr->currentCurrent(),'f',4))
                       /*.arg(QString::number(m_dataMgr->currentPercent()))*/;
            break;
        case TEST_EXCITATION:
            text = tr("励磁测试中,当前电压:%1V,当前电流:%2A")
                       .arg(QString::number(m_dataMgr->currentVolgate(),'f',4))
                       .arg(QString::number(m_dataMgr->currentCurrent(),'f',4))
                       /*.arg(QString::number(m_dataMgr->currentPercent()))*/;
            break;
        case TEST_RATIO:
            text = tr("变比测试中,当前电压:%1V,当前电流:%2A")
                       .arg(QString::number(m_dataMgr->currentVolgate(),'f',4))
                       .arg(QString::number(m_dataMgr->currentCurrent(),'f',4))
                       /*.arg(QString::number(m_dataMgr->currentPercent()))*/;
            break;
        case TEST_BURDEN:
            text = tr("测试负载,当前电压:%1V,当前电流:%2A")
                    .arg(QString::number(m_dataMgr->currentVolgate(),'f',4))
                    .arg(QString::number(m_dataMgr->currentCurrent(),'f',4))
                    /*.arg(QString::number(m_dataMgr->currentPercent()))*/;
            break;
        case TEST_DEMAGNETIOZATION:
            text = tr("正在退磁...");
            break;
        default:
            break;
        }
        m_pStatusLabel->setText(text);
    }
}

void CBottomStatusWidget::initWidget()
{
    m_pStatusLabel = new QLabel(tr("就绪"),this);
    m_pStatusLabel->setMinimumWidth(LABEL_WIDTH);
    m_pStatusLabel->setObjectName("statusLabel");

    m_pUpdateStatusTimer = new QTimer(this);
}

void CBottomStatusWidget::localConnect()
{
    connect(m_pUpdateStatusTimer,SIGNAL(timeout()),this,SLOT(onUpdateStatus()));
}

int CBottomStatusWidget::popSwitchCtrlDlg(const QString &title, const QString &msg)
{
    MQMessageBox messagebox(QMessageBox::Warning,title,msg,QMessageBox::Ok);
    messagebox.setButtonText(QMessageBox::Ok,tr("确定"));
    messagebox.button(QMessageBox::Ok)->setObjectName("ok_messagebox");
    messagebox.setWindowModality(Qt::WindowModal);
    return messagebox.exec();
}

