#include "systemAboutWidget.h"

#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QVBoxLayout>

#include "src/CTTester/dataManager/dataManager.h"

#define LINEEDIT_HEIGHT     30
#define LINEEDIT_WIDTH      250

CSystemAboutWidget::CSystemAboutWidget(QWidget *parent)
    : QWidget(parent)
    , m_dataMgr(CDataManager::getInstance())
{
    initWidget();
}

void CSystemAboutWidget::focusInEvent(QFocusEvent *event)
{
    focusNextChild();

    return QWidget::focusInEvent(event);
}

void CSystemAboutWidget::initWidget()
{
    m_pSystemVersionL = new QLabel(tr("系统版本:  ") + m_dataMgr->systemVersion());
    m_pArithmeticVersionL = new QLabel(tr("算法版本:  ") + m_dataMgr->arithmeticVersion());
    m_pApplicationVersionL = new QLabel(tr("应用版本:  ") + m_dataMgr->applicationVersion());
    m_pFpgaVersionL = new QLabel(tr("FPGA版本:  ") + m_dataMgr->fpgaVersion());
    m_pHardwareVersionL = new QLabel(tr("硬件版本:  ") + m_dataMgr->hardwareVersion());

    m_pAboutGb = new QGroupBox(tr("系统信息"));

    setMainLayout();
    setCtrlsMinSize();
}

void CSystemAboutWidget::setMainLayout()
{
    QFormLayout *groupLayout = new QFormLayout;
    groupLayout->setContentsMargins(5,30,5,10);
    groupLayout->setSpacing(30);
    groupLayout->addRow(m_pApplicationVersionL);
    groupLayout->addRow(m_pArithmeticVersionL);
    groupLayout->addRow(m_pFpgaVersionL);
    groupLayout->addRow(m_pSystemVersionL);
    groupLayout->addRow(m_pHardwareVersionL);
    groupLayout->setFormAlignment(Qt::AlignHCenter);
    m_pAboutGb->setLayout(groupLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_pAboutGb);

    this->setLayout(mainLayout);
}

void CSystemAboutWidget::setCtrlsMinSize()
{
    m_pSystemVersionL->setFixedSize(LINEEDIT_WIDTH,LINEEDIT_HEIGHT);
    m_pArithmeticVersionL->setFixedSize(LINEEDIT_WIDTH,LINEEDIT_HEIGHT);
    m_pApplicationVersionL->setFixedSize(LINEEDIT_WIDTH,LINEEDIT_HEIGHT);
    m_pFpgaVersionL->setFixedSize(LINEEDIT_WIDTH,LINEEDIT_HEIGHT);
    m_pHardwareVersionL->setFixedSize(LINEEDIT_WIDTH,LINEEDIT_HEIGHT);
}

