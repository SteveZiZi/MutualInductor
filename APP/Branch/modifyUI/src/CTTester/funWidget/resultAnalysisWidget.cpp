#include "resultAnalysisWidget.h"

#include <QAction>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QDebug>
#include <QApplication>

#include "src/CTTester/common.h"
#include "src/CTTester/dataManager/dataManager.h"
#include "src/CTTester/funWidget/resultSetting/excitationWidget.h"
#include "src/CTTester/funWidget/resultSetting/ratioWidget.h"
#include "src/CTTester/funWidget/resultSetting/resistanceLoadWidget.h"
#include "src/CTTester/funWidget/resultSetting/saveReportWidget.h"

enum E_BTN_BINDING_WITH_UI
{
    EXCITATION_UI = 0,
    RESISTANCELOAD_UI,
    RATIO_UI,
    SAVEREPORT_UI
};

CResultAnalysisWidget::CResultAnalysisWidget(QWidget *parent)
    : CBasePageWidget(parent)
{
    initWidget();
    initMenu();
    localConnect();
}

void CResultAnalysisWidget::updateResult()
{
    // 电阻
    int testItem = CDataManager::getInstance()->getTestItem();
    if((testItem&TEST_RESISTANCE) || (testItem&TEST_BURDEN)) {
        m_pResistanceLoadWidget->updateWidget();
    }
    else  {
        m_pResistanceLoadWidget->clearTestMark();
    }
    // 励磁
    if(testItem & TEST_EXCITATION) {
        m_pExcitationWidget->updateWidget();
    }
    else {
        m_pExcitationWidget->clearTestMark();
    }
    // 变比
    if(testItem & TEST_RATIO) {
        m_pRatioWidget->updateWidget();
    }
    else {
        m_pRatioWidget->clearTestMark();
    }

    if (testItem & TEST_EXCITATION) {
        m_pStackedWidget->setCurrentIndex(EXCITATION_UI);
    }
    else if (testItem & TEST_RATIO)
        m_pStackedWidget->setCurrentIndex(RATIO_UI);
    else if ((testItem&TEST_RESISTANCE) || (testItem&TEST_BURDEN))
        m_pStackedWidget->setCurrentIndex(RESISTANCELOAD_UI);
    else
        m_pStackedWidget->setCurrentIndex(EXCITATION_UI);
}

void CResultAnalysisWidget::clearResult()
{
    m_pResistanceLoadWidget->clearTestMark();
    // 励磁
    m_pExcitationWidget->clearTestMark();
    // 变比
    m_pRatioWidget->clearTestMark();
}


void CResultAnalysisWidget::displayExcitationResult()
{
    m_pStackedWidget->setCurrentIndex(EXCITATION_UI);
    QKeyEvent *e = new QKeyEvent(QEvent::KeyPress,Qt::Key_Right,Qt::NoModifier);
    QApplication::postEvent(this->parent()->parent(), e);
}

void CResultAnalysisWidget::focusInEvent(QFocusEvent *event)
{
    m_pStackedWidget->currentWidget()->setFocus();

    return QWidget::focusInEvent(event);
}

bool CResultAnalysisWidget::eventFilter(QObject *obj, QEvent *eve)
{
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(eve);
    if(m_pResistanceLoadWidget == obj)
    {
        if(eve->type() == QEvent::KeyPress)
        {
            switch(keyEvent->key())
            {
            case Qt::Key_Right:{
                m_pResistanceLoadWidget->clearFocus();
                QKeyEvent *e = new QKeyEvent(keyEvent->type(),keyEvent->key(),Qt::NoModifier);
                QApplication::postEvent(this->parent()->parent(), e);
                return false;
            }
            default:
                break;
            }
        }
    }
    else if(m_pRatioWidget == obj)
    {
        if(eve->type() == QEvent::KeyPress)
        {
            switch (keyEvent->key())
            {
            case Qt::Key_Right:{
                m_pRatioWidget->clearFocus();
                QKeyEvent *e = new QKeyEvent(keyEvent->type(),keyEvent->key(),Qt::NoModifier);
                QApplication::postEvent(this->parent()->parent(), e);
                return false;
            }
            default:
                break;
            }
        }
    }
    else if(m_pSaveReportWidget == obj)
    {
        if(eve->type() == QEvent::KeyPress)
        {
            switch (keyEvent->key())
            {
            case Qt::Key_Right:{
                m_pSaveReportWidget->clearFocus();
                QKeyEvent *e = new QKeyEvent(keyEvent->type(),keyEvent->key(),Qt::NoModifier);
                QApplication::postEvent(this->parent()->parent(), e);
                return false;
            }
            default:
                break;
            }
        }
    }
    else if(m_pExcitationWidget == obj)
    {
        if(eve->type() == QEvent::KeyPress)
        {
            switch (keyEvent->key())
            {
            case Qt::Key_Right:{
                m_pExcitationWidget->clearFocus();
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

void CResultAnalysisWidget::actionTrigger()
{
    QList<QAction*> actions = this->actions();
    if(actions[EXCITATION_UI] == sender())
    {
        m_pStackedWidget->setCurrentIndex(EXCITATION_UI);
    }
    else if(actions[RESISTANCELOAD_UI] == sender())
    {
        m_pStackedWidget->setCurrentIndex(RESISTANCELOAD_UI);
    }
    else if(actions[RATIO_UI] == sender())
    {
        m_pStackedWidget->setCurrentIndex(RATIO_UI);
    }
    else if(actions[SAVEREPORT_UI] == sender())
    {
        m_pStackedWidget->setCurrentIndex(SAVEREPORT_UI);
    }
}

void CResultAnalysisWidget::initWidget()
{
    m_pStackedWidget = new QStackedWidget(this);
    m_pStackedWidget->setFixedSize(STACK_WIDGET_W,STACK_WIDGET_H);
    m_pExcitationWidget = new CExcitationWidget;                           // 励磁数据
    m_pResistanceLoadWidget = new CResistanceLoadWidget;                   // 电阻负荷
    m_pRatioWidget = new CRatioWidget;                                     // 变比数据
    m_pSaveReportWidget = new CSaveReportWidget;                           // 保存报告

    m_pStackedWidget->addWidget(m_pExcitationWidget);
    m_pStackedWidget->addWidget(m_pResistanceLoadWidget);
    m_pStackedWidget->addWidget(m_pRatioWidget);
    m_pStackedWidget->addWidget(m_pSaveReportWidget);

    setMainLayout();
}

void CResultAnalysisWidget::initMenu()
{
    this->addAction(new QAction(tr("励磁曲线"), this));
    this->addAction(new QAction(tr("电阻负荷"), this));
    this->addAction(new QAction(tr("变比数据"), this));
    this->addAction(new QAction(tr("保存报告"), this));
}

void CResultAnalysisWidget::setMainLayout()
{
    m_pStackedWidget->setGeometry(QRect(STACK_WIDGET_SHOW_L,STACK_WIDGET_SHOW_T,
                                  STACK_WIDGET_SHOW_W,STACK_WIDGET_SHOW_H));
}

void CResultAnalysisWidget::localConnect()
{
    QList<QAction*> actions = this->actions();
    connect(actions[EXCITATION_UI],SIGNAL(triggered()),this,SLOT(actionTrigger()));
    connect(actions[RESISTANCELOAD_UI],SIGNAL(triggered()),this,SLOT(actionTrigger()));
    connect(actions[RATIO_UI],SIGNAL(triggered()),this,SLOT(actionTrigger()));
    connect(actions[SAVEREPORT_UI],SIGNAL(triggered()),this,SLOT(actionTrigger()));
}

void CResultAnalysisWidget::addEventFilter()
{
    m_pExcitationWidget->installEventFilter(this);
    m_pRatioWidget->installEventFilter(this);
    m_pResistanceLoadWidget->installEventFilter(this);
    m_pSaveReportWidget->installEventFilter(this);
}

