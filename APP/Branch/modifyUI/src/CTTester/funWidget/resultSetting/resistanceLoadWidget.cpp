#include "resistanceLoadWidget.h"

#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QFormLayout>

#include "src/CTTester/dataManager/dataManager.h"

#define     MIN_LINEEDIT_HEIGHT     30

CResistanceLoadWidget::CResistanceLoadWidget(QWidget *parent)
    : QWidget(parent)
    , m_dataMgr(CDataManager::getInstance())
{
    initWidget();
}

void CResistanceLoadWidget::clearTestMark()
{
    m_pLoadData->setText("");                 // 实测负载
    m_pPowerFactor->setText("");              // 功率因数
    m_pImpedance->setText("");                // 阻抗
}

void CResistanceLoadWidget::updateWidget()
{
    if(m_dataMgr->getResult())
    {
        m_pLoadData->setText(QString::number(m_dataMgr->ctPtResult().burden.measBurden,'f',4));                 // 实测负载
        m_pPowerFactor->setText(QString::number(m_dataMgr->ctPtResult().burden.powerFactor,'f',4));             // 功率因数
        m_pImpedance->setText(QString::number(m_dataMgr->ctPtResult().burden.Z,'f',4));                         // 阻抗
        m_pResistance_25->setText(QString::number(m_dataMgr->ctPtResult().widingRes.measRes,'f',4));            // 测量电阻
        m_pResistance_75->setText(QString::number(m_dataMgr->ctPtResult().widingRes.refRes,'f',4));             // 参考电阻
    }
    else
    {
        clearTestMark();
    }
}

void CResistanceLoadWidget::focusInEvent(QFocusEvent *event)
{
    focusNextChild();

    return QWidget::focusInEvent(event);
}

void CResistanceLoadWidget::initWidget()
{
    m_pResistance_25 = new QLineEdit;                          // 25℃
    m_pResistance_75 = new QLineEdit;                          // 75℃
    m_pLoadData = new QLineEdit;                               // 实测负载
    m_pPowerFactor = new QLineEdit;                            // 功率因数
    m_pImpedance = new QLineEdit;                              // 阻抗

    m_pResistanceGb = new QGroupBox(tr("直阻"));
    m_pLoadGb = new QGroupBox(tr("负载"));

    m_pResistance_25->setMinimumHeight(MIN_LINEEDIT_HEIGHT);                          // 25℃
    m_pResistance_75->setMinimumHeight(MIN_LINEEDIT_HEIGHT);                          // 75℃
    m_pLoadData->setMinimumHeight(MIN_LINEEDIT_HEIGHT);                               // 实测负载
    m_pPowerFactor->setMinimumHeight(MIN_LINEEDIT_HEIGHT);                            // 功率因数
    m_pImpedance->setMinimumHeight(MIN_LINEEDIT_HEIGHT);                              // 阻抗

    setMainLayout();
    setLineEditEnable(false);
}

void CResistanceLoadWidget::setMainLayout()
{
    QFormLayout *leftGbLayout = new QFormLayout;
    leftGbLayout->setContentsMargins(15,40,15,0);
    leftGbLayout->setSpacing(30);
    leftGbLayout->addRow(new QLabel(tr("温度")),new QLabel(tr("阻值")));
    leftGbLayout->addRow(new QLabel(tr("25℃:")),m_pResistance_25);
    leftGbLayout->addRow(new QLabel(tr("75℃:")),m_pResistance_75);
    m_pResistanceGb->setLayout(leftGbLayout);

    QFormLayout *rightGbLayout = new QFormLayout;
    rightGbLayout->setContentsMargins(5,40,15,0);
    rightGbLayout->setSpacing(30);
    rightGbLayout->addRow(new QLabel(tr("实测负载:")),m_pLoadData);
    rightGbLayout->addRow(new QLabel(tr("功率因数:")),m_pPowerFactor);
    rightGbLayout->addRow(new QLabel(tr("阻抗:")),m_pImpedance);
    m_pLoadGb->setLayout(rightGbLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_pResistanceGb);
    mainLayout->addWidget(m_pLoadGb);

    this->setLayout(mainLayout);
}

void CResistanceLoadWidget::setLineEditEnable(bool enable)
{
    m_pResistance_25->setEnabled(enable);                          // 25℃
    m_pResistance_75->setEnabled(enable);                          // 75℃
    m_pLoadData->setEnabled(enable);                               // 实测负载
    m_pPowerFactor->setEnabled(enable);                            // 功率因数
    m_pImpedance->setEnabled(enable);                              // 阻抗
}

