#include "funTabWidget.h"

#include <QDebug>
#include <QAction>

#include "src/CTTester/common.h"
#include "src/CTTester/funWidget/paramSettingWidget.h"
#include "src/CTTester/funWidget/resultAnalysisWidget.h"
#include "src/CTTester/funWidget/systemSettingWidget.h"
#include "src/CTTester/menuBar/menuBar.h"
#include "src/CTTester/dataManager/dataManager.h"


#define    PARAMSETTINGWIDGET_ICON_PATH     ":/resource/images/para.png"
#define    RESULTANALISISWIDGET_ICON_PATH   ":/resource/images/analysis.png"
#define    SYSTEMSETTINGWIDGET_ICON_PATH    ":/resource/images/para.png"
#define    HELPWIDGET_ICON_PATH             ":/resource/images/para.png"

enum
{
    PARAM_SETTING_WIDGET = 0,
    RESULT_ANALYSIS_WIDGET,
    SYSTEM_SETTING_WIDGET,
    HELP_WIDGET
};

CFunTabWidget::CFunTabWidget(QWidget *parent)
    : MQTabWidget(parent)
{
    initFunTabWidget();
    loaclConnect();
}

CFunTabWidget::~CFunTabWidget()
{

}

void CFunTabWidget::focusInEvent(QFocusEvent *event)
{
    focusNextChild();

    return QTabWidget::focusInEvent(event);
}

void CFunTabWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_F6 || event->key() == Qt::Key_F7)
    {
        if(PARAM_SETTING_WIDGET == currentIndex())
        {
            m_pParamSettingWidget->keyPressEvent(event);
        }
    }
    else
    {
        MQTabWidget::keyPressEvent(event);
    }
}

void CFunTabWidget::onCurrentChanged(int index)
{
    switch (index)
    {
    case PARAM_SETTING_WIDGET:
        break;
    case RESULT_ANALYSIS_WIDGET:
        m_pResultAnalysisWidget->updateResult();
        break;
    case SYSTEM_SETTING_WIDGET:
        break;
    case HELP_WIDGET:
        break;
    default:
        break;
    }
    this->setFocus();
}

void CFunTabWidget::onEnableFunTab(bool status)
{
    if(m_pParamSettingWidget == sender())
    {
        for(int i = RESULT_ANALYSIS_WIDGET;i < this->count();i++)
        {
            this->setTabEnabled(i,status);
        }
        for(int i = START_BUTTON;i < m_pParamSettingWidget->btnsCount();i++)
        {
            if(STOP_BUTTON == i)
                continue;
            else
                GetMenuBar()->changeActionsStatus(i);
        }
    }
    else if(m_pSystemSettingWidget == sender())
    {
        for(int i = 0;i < this->count();i++)
        {
            if(SYSTEM_SETTING_WIDGET == i)
            {
                continue;
            }
            this->setTabEnabled(i,status);
        }
        GetMenuBar()->changeActionsStatus();
    }
}

void CFunTabWidget::onDisplayResult()
{
    setCurrentIndex(RESULT_ANALYSIS_WIDGET);
    m_pResultAnalysisWidget->displayExcitationResult();
}

void CFunTabWidget::initFunTabWidget()
{
    m_pParamSettingWidget = new CParamSettingWidget(this);
    m_pResultAnalysisWidget = new CResultAnalysisWidget(this);
    m_pSystemSettingWidget = new CSystemSettingWidget(this);
//    m_pHelpWidget = new CHelpWidget(this);

    this->addTab(m_pParamSettingWidget,QIcon(PARAMSETTINGWIDGET_ICON_PATH),tr("参数设置"));
    this->addTab(m_pResultAnalysisWidget,QIcon(RESULTANALISISWIDGET_ICON_PATH),tr("结果分析"));
    this->addTab(m_pSystemSettingWidget,QIcon(SYSTEMSETTINGWIDGET_ICON_PATH),tr("系统设置"));
//    this->addTab(m_pHelpWidget,QIcon(HELPWIDGET_ICON_PATH),tr("帮助文档"));

    this->resize(FUNC_PAGE_W,FUNC_PAGE_H);
    this->setIconSize(QSize(30,30));
    this->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    this->setObjectName("funTab");
}

void CFunTabWidget::loaclConnect()
{
    connect(this,&CFunTabWidget::currentChanged,this,&CFunTabWidget::onCurrentChanged);
    connect(m_pParamSettingWidget,&CParamSettingWidget::enableFunTab,
            this,&CFunTabWidget::onEnableFunTab);
    connect(m_pSystemSettingWidget,&CSystemSettingWidget::enableFunTab,
            this,&CFunTabWidget::onEnableFunTab);
    connect(m_pParamSettingWidget,&CParamSettingWidget::displayResult,
            this,&CFunTabWidget::onDisplayResult);
}

