#include "systemSelfTestWidget.h"

#include <QGroupBox>
#include <QLabel>
#include <QBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QKeyEvent>

#include "src/CTTester/dataManager/dataManager.h"
#include "src/CTTester/overrideControl/mPushButton.h"
#include "src/CTTester/overrideControl/mqmessagebox.h"

#define LINEEDIT_HEIGHT     30
#define LINEEDIT_WIDTH      250
#define BUTTON_HEIGHT       30
#define BUTTON_WIDTH        160

CSystemSelfTestWidget::CSystemSelfTestWidget(QWidget *parent)
    : QWidget(parent),
      m_dataMgr(CDataManager::getInstance()),
      m_bMessageBoxPoped(false)
{
    initWidget();
    localConnect();
    addEventFilter();
}

void CSystemSelfTestWidget::focusInEvent(QFocusEvent *event)
{
    focusNextChild();

    return QWidget::focusInEvent(event);
}

bool CSystemSelfTestWidget::eventFilter(QObject *obj, QEvent *eve)
{
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(eve);
    if(m_pTestCurrent == obj || m_pTestVolgate == obj || m_pTestFrequeny ||
       m_pActuallCurrent == obj || m_pActuallVolgate == obj || m_pStartTestBtn == obj ||
       m_pStopTestBtn == obj)
    {
        if(eve->type() == QEvent::KeyPress)
        {
            switch(keyEvent->key())
            {
            case Qt::Key_Up:
                focusPreviousChild();
                return true;
            case Qt::Key_Left:
                if(m_pStopTestBtn == obj)
                    m_pStartTestBtn->setFocus();
                else
                    focusPreviousChild();
                return true;
            case Qt::Key_Down:
                focusNextChild();
                return true;
            case Qt::Key_Right:
                if(m_pStartTestBtn == obj)
                    m_pStopTestBtn->setFocus();
                else
                    this->parentWidget()->parentWidget()->eventFilter(this,eve);
                return true;
            default:
                break;
            }
        }
    }

    return false;
}

void CSystemSelfTestWidget::onBtnClicked()
{
    if(m_pStartTestBtn == sender())
    {
        if(m_bMessageBoxPoped)
        {
            return;
        }

        m_bMessageBoxPoped = true;

        if(popSwitchCtrlDlg(tr("开始测试?"), tr("是否开始进行测试?")) == QMessageBox::Cancel)
        {
            m_bMessageBoxPoped = false;
            return;
        }
        else
        {
            m_pStartTestBtn->setEnabled(false);

            emit enableFunTab(false);

            m_bMessageBoxPoped = false;
        }
    }
    else if(m_pStopTestBtn == sender())
    {
        if(!m_pStartTestBtn->isEnabled()){
            emit enableFunTab(true);
            m_pStartTestBtn->setEnabled(true);
        }
    }
}

void CSystemSelfTestWidget::initWidget()
{
    m_pTestCurrent = new QLineEdit();                                // 测试电流
    m_pTestVolgate = new QLineEdit();                                // 测试电压
    m_pTestFrequeny = new QLineEdit();                               // 测试频率
    m_pActuallCurrent = new QLineEdit();                             // 实际电流
    m_pActuallVolgate = new QLineEdit();                             // 实际电压
    m_pStartTestBtn = new MPushButton(tr("开始测试"));                           // 开始测试
    m_pStopTestBtn = new MPushButton(tr("停止测试"));                            // 停止测试
    m_pSysSelfTestGb = new QGroupBox(tr("系统自测"));

    setMainLayout();
    setCtrlsMinSize();
}

void CSystemSelfTestWidget::setMainLayout()
{
    QFormLayout *groupLayout = new QFormLayout;
    groupLayout->setContentsMargins(5,20,5,10);
    groupLayout->setSpacing(20);
    groupLayout->addRow(new QLabel(tr("测试电流:")),m_pTestCurrent);
    groupLayout->addRow(new QLabel(tr("测试电压:")),m_pTestVolgate);
    groupLayout->addRow(new QLabel(tr("测试频率:")),m_pTestFrequeny);
    groupLayout->addRow(new QLabel(tr("实际电流:")),m_pActuallCurrent);
    groupLayout->addRow(new QLabel(tr("实际电压:")),m_pActuallVolgate);
    groupLayout->setFormAlignment(Qt::AlignCenter|Qt::AlignTop);
    m_pSysSelfTestGb->setLayout(groupLayout);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(m_pStartTestBtn);
    btnLayout->addWidget(m_pStopTestBtn);
    btnLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_pSysSelfTestGb);
    mainLayout->addLayout(btnLayout);

    this->setLayout(mainLayout);
}

void CSystemSelfTestWidget::setCtrlsMinSize()
{
    m_pTestCurrent->setFixedSize(LINEEDIT_WIDTH,LINEEDIT_HEIGHT);                            // 测试电流
    m_pTestVolgate->setFixedSize(LINEEDIT_WIDTH,LINEEDIT_HEIGHT);                            // 测试电压
    m_pTestFrequeny->setFixedSize(LINEEDIT_WIDTH,LINEEDIT_HEIGHT);                           // 测试频率
    m_pActuallCurrent->setFixedSize(LINEEDIT_WIDTH,LINEEDIT_HEIGHT);                         // 实际电流
    m_pActuallVolgate->setFixedSize(LINEEDIT_WIDTH,LINEEDIT_HEIGHT);                         // 实际电压
    m_pStartTestBtn->setFixedSize(BUTTON_WIDTH,BUTTON_HEIGHT);                           // 开始测试
    m_pStopTestBtn->setFixedSize(BUTTON_WIDTH,BUTTON_HEIGHT);                            // 停止测试
}

void CSystemSelfTestWidget::localConnect()
{
    connect(m_pStartTestBtn,SIGNAL(clicked()),this,SLOT(onBtnClicked()));
    connect(m_pStopTestBtn,SIGNAL(clicked()),this,SLOT(onBtnClicked()));
}

void CSystemSelfTestWidget::addEventFilter()
{
    m_pTestCurrent->installEventFilter(this);                            // 测试电流
    m_pTestVolgate->installEventFilter(this);                            // 测试电压
    m_pTestFrequeny->installEventFilter(this);                           // 测试频率
    m_pActuallCurrent->installEventFilter(this);                         // 实际电流
    m_pActuallVolgate->installEventFilter(this);                         // 实际电压
    m_pStartTestBtn->installEventFilter(this);                           // 开始测试
    m_pStopTestBtn->installEventFilter(this);                            // 停止测试
}

int CSystemSelfTestWidget::popSwitchCtrlDlg(const QString &title, const QString &msg)
{
    MQMessageBox messagebox(QMessageBox::Warning,title,msg,QMessageBox::Ok|QMessageBox::Cancel);
    messagebox.setButtonText(QMessageBox::Ok,tr("确定"));
    messagebox.button(QMessageBox::Ok)->setObjectName("ok_messagebox");
    messagebox.setButtonText(QMessageBox::Cancel,tr("取消"));
    messagebox.button(QMessageBox::Cancel)->setObjectName("cancel_messagebox");
    messagebox.setWindowModality(Qt::WindowModal);
    return messagebox.exec();
}

