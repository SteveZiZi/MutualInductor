#include "saveReportWidget.h"

#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLineEdit>

#include "src/CTTester/overrideControl/mqcheckbox.h"
#include "src/CTTester/overrideControl/mPushButton.h"
#include "src/CTTester/dataManager/dataManager.h"


#define MIN_CONTROL_HEIGHT      30
#define GROUPBOX_FIX_WIDTH      400

CSaveReportWidget::CSaveReportWidget(QWidget *parent)
    : QWidget(parent)
    , m_dataMgr(CDataManager::getInstance())
{
    initWidget();
    localConnect();
    addEventFilter();
}

void CSaveReportWidget::focusInEvent(QFocusEvent *event)
{
    focusNextChild();

    return QWidget::focusInEvent(event);
}

bool CSaveReportWidget::eventFilter(QObject *obj, QEvent *eve)
{
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(eve);
    if(m_pReportName == obj || m_pTestTime == obj || m_pExcitationData == obj ||
       m_pResistanceData == obj || m_pRatioData == obj || m_pLoadData == obj ||
       m_pSaveBtn == obj)
    {
        if(eve->type() == QEvent::KeyPress)
        {
            switch(keyEvent->key())
            {
            case Qt::Key_Up:
                if(m_pRatioData == obj)
                    m_pResistanceData->setFocus();
                else if(m_pLoadData == obj)
                    m_pExcitationData->setFocus();
                else
                    focusPreviousChild();
                return true;
            case Qt::Key_Down:
                if(m_pResistanceData == obj)
                    m_pRatioData->setFocus();
                else if(m_pExcitationData == obj)
                    m_pLoadData->setFocus();
                else if(m_pLoadData == obj || m_pRatioData == obj)
                    m_pSaveBtn->setFocus();
                else
                    focusNextChild();
                return true;
            case Qt::Key_Left:
                if(m_pExcitationData == obj)
                    m_pResistanceData->setFocus();
                else if(m_pLoadData == obj)
                    m_pRatioData->setFocus();
                return true;
            case Qt::Key_Right:
                if(m_pResistanceData == obj)
                    m_pExcitationData->setFocus();
                else if(m_pRatioData == obj)
                    m_pLoadData->setFocus();
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

void CSaveReportWidget::initWidget()
{
    //报告区域
    m_pReportGb = new QGroupBox(tr("保存报告"));
    m_pReportGb->setFixedWidth(GROUPBOX_FIX_WIDTH);
    m_pReportName = new QLineEdit();
    m_pTestTime = new QLineEdit();
    m_pReportName->setMinimumHeight(MIN_CONTROL_HEIGHT);
    m_pTestTime->setMinimumHeight(MIN_CONTROL_HEIGHT);
    //保存项目
    m_pResistanceData = new MQCheckBox(tr("电阻数据"));
    m_pExcitationData = new MQCheckBox(tr("励磁数据"));
    m_pRatioData = new MQCheckBox(tr("变比数据"));
    m_pLoadData = new MQCheckBox(tr("负荷数据"));
    m_pSaveItemGb = new QGroupBox(tr("保存内容选项"));
    m_pSaveItemGb->setFixedWidth(GROUPBOX_FIX_WIDTH);
    m_pResistanceData->setMinimumHeight(MIN_CONTROL_HEIGHT);
    m_pExcitationData->setMinimumHeight(MIN_CONTROL_HEIGHT);
    m_pRatioData->setMinimumHeight(MIN_CONTROL_HEIGHT);
    m_pLoadData->setMinimumHeight(MIN_CONTROL_HEIGHT);

    m_pSaveBtn = new MPushButton(tr("保存"));
    m_pSaveBtn->setMinimumSize(100,40);

    setMainLayout();
}

void CSaveReportWidget::setMainLayout()
{
    QFormLayout *topGbLayout = new QFormLayout;
    topGbLayout->setSpacing(10);
    topGbLayout->setContentsMargins(20,30,20,10);
    topGbLayout->addRow(new QLabel(tr("报告名称:")),m_pReportName);
    topGbLayout->addRow(new QLabel(tr("试验日期:")),m_pTestTime);
    m_pReportGb->setLayout(topGbLayout);
    m_pReportGb->setAlignment(Qt::AlignCenter);

    QHBoxLayout *resAndExcLayout = new QHBoxLayout;
    resAndExcLayout->addWidget(m_pResistanceData);
    resAndExcLayout->addSpacing(10);
    resAndExcLayout->addWidget(m_pExcitationData);
    resAndExcLayout->addStretch();

    QHBoxLayout *ratioAndLoadLayout = new QHBoxLayout;
    ratioAndLoadLayout->addWidget(m_pRatioData);
    ratioAndLoadLayout->addSpacing(10);
    ratioAndLoadLayout->addWidget(m_pLoadData);
    ratioAndLoadLayout->addStretch();

    QVBoxLayout *buttomGbLayout = new QVBoxLayout;
    buttomGbLayout->setSpacing(10);
    buttomGbLayout->setContentsMargins(20,30,20,10);
    buttomGbLayout->addLayout(resAndExcLayout);
    buttomGbLayout->addLayout(ratioAndLoadLayout);
    m_pSaveItemGb->setLayout(buttomGbLayout);
    m_pSaveItemGb->setAlignment(Qt::AlignCenter);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_pReportGb);
    mainLayout->addWidget(m_pSaveItemGb);
    mainLayout->addWidget(m_pSaveBtn);
    mainLayout->setAlignment(m_pReportGb,Qt::AlignHCenter);
    mainLayout->setAlignment(m_pSaveItemGb,Qt::AlignHCenter);
    mainLayout->setAlignment(m_pSaveBtn,Qt::AlignHCenter);

    this->setLayout(mainLayout);
}

void CSaveReportWidget::localConnect()
{

}

void CSaveReportWidget::addEventFilter()
{
    m_pReportName->installEventFilter(this);
    m_pTestTime->installEventFilter(this);
    m_pResistanceData->installEventFilter(this);
    m_pExcitationData->installEventFilter(this);
    m_pRatioData->installEventFilter(this);
    m_pLoadData->installEventFilter(this);
    m_pSaveBtn->installEventFilter(this);
}

