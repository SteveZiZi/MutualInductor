#include "systemSettingWidget.h"

#include <QStackedWidget>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QAction>
#include <QApplication>

#include "src/CTTester/common.h"
#include "src/CTTester/funWidget/systemSetting/systemDebugWidget.h"
#include "src/CTTester/funWidget/systemSetting/systemSelfTestWidget.h"
#include "src/CTTester/funWidget/systemSetting/systemUpdateWidget.h"
#include "src/CTTester/funWidget/systemSetting/systemAboutWidget.h"

enum E_SYSTEM_INDEX
{
    SYSTEM_SELF_TEST = 0,
    SYSTEM_DEBUG,
    SYSTEM_UPDATE,
    SYSTEM_ABOUT
};

CSystemSettingWidget::CSystemSettingWidget(QWidget *parent)
    : CBasePageWidget(parent)
{
    initWidget();
    initMenu();
    localConnect();
    addEventFilter();
}

void CSystemSettingWidget::focusInEvent(QFocusEvent *event)
{
    m_pStackedWidget->currentWidget()->setFocus();

    return QWidget::focusInEvent(event);
}

bool CSystemSettingWidget::eventFilter(QObject *obj, QEvent *eve)
{
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(eve);

    if(m_pSystemSelfTestWidget == obj)
    {
        if(eve->type() == QEvent::KeyPress)
        {
            switch(keyEvent->key())
            {
            case Qt::Key_Right:{
                m_pSystemSelfTestWidget->clearFocus();
                QKeyEvent *e = new QKeyEvent(keyEvent->type(),keyEvent->key(),Qt::NoModifier);
                QApplication::postEvent(this->parent()->parent(), e);
                return false;
            }
            default:
                break;
            }
        }
    }
    else if(m_pSystemDebugWidget == obj)
    {
        if(eve->type() == QEvent::KeyPress)
        {
            switch(keyEvent->key())
            {
            case Qt::Key_Right:{
                m_pSystemDebugWidget->clearFocus();
                QKeyEvent *e = new QKeyEvent(keyEvent->type(),keyEvent->key(),Qt::NoModifier);
                QApplication::postEvent(this->parent()->parent(), e);
                return false;
            }
            default:
                break;
            }
        }
    }
    else if(m_pSystemUpdateWidget == obj)
    {
        if(eve->type() == QEvent::KeyPress)
        {
            switch(keyEvent->key())
            {
            case Qt::Key_Right:{
                m_pSystemUpdateWidget->clearFocus();
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

void CSystemSettingWidget::actionTrigger()
{
    QList<QAction*> actions = this->actions();
    if(actions[SYSTEM_SELF_TEST] == sender())
    {
        m_pStackedWidget->setCurrentIndex(SYSTEM_SELF_TEST);
    }
    else if(actions[SYSTEM_DEBUG] == sender())
    {
        m_pStackedWidget->setCurrentIndex(SYSTEM_DEBUG);
    }
    else if(actions[SYSTEM_UPDATE] == sender())
    {
        m_pStackedWidget->setCurrentIndex(SYSTEM_UPDATE);
    }
    else if(actions[SYSTEM_ABOUT] == sender())
    {
        m_pStackedWidget->setCurrentIndex(SYSTEM_ABOUT);
    }
}

void CSystemSettingWidget::onEnableFunTab(bool status)
{
    emit enableFunTab(status);
}

void CSystemSettingWidget::initWidget()
{
    m_pStackedWidget = new QStackedWidget(this);                                                // 堆栈窗口
    m_pStackedWidget->setFixedSize(STACK_WIDGET_W,STACK_WIDGET_H);

    m_pSystemSelfTestWidget = new CSystemSelfTestWidget();                                      // 系统自测
    m_pSystemDebugWidget = new CSystemDebugWidget;                                              // 系统调试
    m_pSystemUpdateWidget = new CSystemUpdateWidget;                                            // 系统升级
    m_pSystemAboutWidget = new CSystemAboutWidget();                                            // 系统信息

    m_pStackedWidget->addWidget(m_pSystemSelfTestWidget);
    m_pStackedWidget->addWidget(m_pSystemDebugWidget);
    m_pStackedWidget->addWidget(m_pSystemUpdateWidget);
    m_pStackedWidget->addWidget(m_pSystemAboutWidget);

    setMainLayout();
}

void CSystemSettingWidget::initMenu()
{
    this->addAction(new QAction(tr("系统自测"), this));
    this->addAction(new QAction(tr("系统调试"), this));
    this->addAction(new QAction(tr("系统升级"), this));
    this->addAction(new QAction(tr("系统信息"), this));
}

void CSystemSettingWidget::setMainLayout()
{
    m_pStackedWidget->setGeometry(QRect(STACK_WIDGET_SHOW_L,STACK_WIDGET_SHOW_T,
                                  STACK_WIDGET_SHOW_W,STACK_WIDGET_SHOW_H));
}

void CSystemSettingWidget::localConnect()
{
    QList<QAction*> actions = this->actions();
    connect(actions[SYSTEM_SELF_TEST],SIGNAL(triggered()),this,SLOT(actionTrigger()));
    connect(actions[SYSTEM_DEBUG],SIGNAL(triggered()),this,SLOT(actionTrigger()));
    connect(actions[SYSTEM_UPDATE],SIGNAL(triggered()),this,SLOT(actionTrigger()));
    connect(actions[SYSTEM_ABOUT],SIGNAL(triggered()),this,SLOT(actionTrigger()));

    // 系统自测
    connect(m_pSystemSelfTestWidget,&CSystemSelfTestWidget::enableFunTab,
            this,&CSystemSettingWidget::onEnableFunTab);
}

void CSystemSettingWidget::addEventFilter()
{

}

