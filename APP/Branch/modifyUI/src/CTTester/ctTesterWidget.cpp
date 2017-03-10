#include "ctTesterWidget.h"

#include <QTabWidget>
#include <QTimer>
#include <QDateTime>
#include <QLabel>
#include <QKeyEvent>
#include <QApplication>

#include "src/CTTester/common.h"
#include "funWidget/funTabWidget.h"
#include "src/CTTester/menuBar/menuBar.h"


CCtTesterWidget::CCtTesterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ctTesterWidget)
{
    ui->setupUi(this);
    initWidget();
}

CCtTesterWidget::~CCtTesterWidget()
{
    delete ui;
}

void CCtTesterWidget::keyPressEvent(QKeyEvent *e)
{
    switch(e->key()) {
    case Qt::Key_F1:
        qApp->exit(0);
        e->accept();
        break;
    case Qt::Key_Escape:
        m_pTabWidget->setFocus();
        e->accept();
        break;
    case Qt::Key_Left:
        m_pTabWidget->currentWidget()->setFocus();
        e->accept();
        break;
    case Qt::Key_Right:
        m_pMenuBar->setFocus();
        e->accept();
        break;
    default:
        QWidget::keyPressEvent(e);
        break;
    }
}

void CCtTesterWidget::clockTimerSlot()
{
    QDateTime sysTime = QDateTime::currentDateTime();
    m_pClockLabel->setText(tr("CT测试仪 ") + sysTime.toString("yyyy-MM-dd hh:mm:ss"));
}

void CCtTesterWidget::initWidget()
{
    this->setWindowFlags(Qt::FramelessWindowHint);                              // 隐藏边框

    m_pMenuBar = GetMenuBar();
    m_pMenuBar->setParent(this);
    m_pMenuBar->move(MENU_BAR_L, MENU_BAR_T);
    m_pMenuBar->resize(MENU_BAR_W, MENU_BAR_H);


    m_pTabWidget = new CFunTabWidget(this);
    connect(m_pTabWidget, SIGNAL(actionsChanged(QList<QAction*>)), m_pMenuBar, SLOT(updateActions(QList<QAction*>)));
    connect(m_pTabWidget, SIGNAL(actionsStatusChanged(int)), m_pMenuBar, SLOT(changeActionsStatus(int)));
    connect(m_pTabWidget, SIGNAL(btnsStatusChanged()), m_pMenuBar, SLOT(changeActionsStatus()));
    m_pMenuBar->updateActions(m_pTabWidget->widget(0)->actions());

    m_pClockTimer = new QTimer(this);
    m_pClockTimer->start(500);
    connect(m_pClockTimer,SIGNAL(timeout()),this,SLOT(clockTimerSlot()));

    m_pClockLabel = new QLabel(this);
    m_pClockLabel->setMinimumSize(250,30);                                      //设置标签最小尺寸
    m_pClockLabel->setFrameShape(QFrame::NoFrame);                              //设置标签形状
    m_pClockLabel->setGeometry(QRect(550,0,200,30));

}
