#include "testItemWidget.h"

#include <QHBoxLayout>
#include <QDebug>
#include <QApplication>

#include "src/CTTester/dataManager/dataManager.h"
#include "src/CTTester/dataManager/device/paramdef.h"
#include "src/CTTester/overrideControl/mqcheckbox.h"


CTestItemWidget::CTestItemWidget(QWidget *parent) : QGroupBox(parent)
{
    this->setTitle(tr("试验项目"));

    initWidget();
    setDefaultValue();
    addEventFilter();
}

void CTestItemWidget::inductorChang(int type)
{
    if(type)
        onVoltageType();
    else
        onCurrentType();
}

void CTestItemWidget::focusInEvent(QFocusEvent * event)
{
    focusNextChild();

    return QWidget::focusInEvent(event);
}

bool CTestItemWidget::eventFilter(QObject *obj, QEvent *eve)
{
    if(m_pExcitationCheck == obj || m_pLoadCheck == obj ||
       m_pChangeRateCheck == obj || m_pResistanceCheck == obj)
    {
        if(eve->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(eve);
            switch (keyEvent->key())
            {
            case Qt::Key_Up:
                this->parentWidget()->eventFilter(this,eve);
                return true;
            case Qt::Key_Down:
                this->parentWidget()->eventFilter(this,eve);
                return true;
            case Qt::Key_Left:
                focusPreviousChild();
                return true;
            case Qt::Key_Right:
                if(m_pLoadCheck->isVisible()){
                    if(m_pLoadCheck == obj){
                        this->parentWidget()->eventFilter(this,eve);
                        return true;
                    }
                }
                else
                    if(m_pChangeRateCheck == obj){
                        this->parentWidget()->eventFilter(this,eve);
                        return true;
                    }
                focusNextChild();
                return true;
            default:
                break;
            }
        }
    }

    return false;
}

void CTestItemWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Escape:
    {
        QKeyEvent *e = new QKeyEvent(event->type(),event->key(),Qt::NoModifier);
        QApplication::postEvent(this->parent(), e);
        event->ignore();
        break;
    }
    default:
        break;
    }
}

void CTestItemWidget::onCheckBoxClicked(int status)
{
    int item = CDataManager::getInstance()->getTestItem();
    int testItem = item;

    if(Qt::Checked == status) {
        if(m_pResistanceCheck == sender()) {
            testItem &= ~TEST_BURDEN;
            testItem |= TEST_RESISTANCE;
        }
        else if(m_pExcitationCheck == sender()) {
            testItem &= ~TEST_BURDEN;
            testItem |= TEST_EXCITATION|TEST_RESISTANCE;
        }
        else if(m_pChangeRateCheck == sender()) {
            testItem = TEST_RATIO | TEST_EXCITATION | TEST_RESISTANCE;
        }
        else if(m_pLoadCheck == sender()) {
            testItem = TEST_BURDEN;
        }
    }
    else if(Qt::Unchecked == status)
    {
        if(m_pResistanceCheck == sender()) {
            if (testItem&TEST_BURDEN)
                testItem = TEST_BURDEN;
            else
                testItem = TEST_RESISTANCE;
        }
        else if(m_pExcitationCheck == sender()) {
            testItem &= ~(TEST_EXCITATION|TEST_RATIO);
        }
        else if(m_pChangeRateCheck == sender()) {
            testItem &= ~TEST_RATIO;
        }
        else if(m_pLoadCheck == sender()) {
            testItem &= ~TEST_BURDEN;
            testItem |= TEST_RESISTANCE;
        }
    }

    CDataManager::getInstance()->setTestItem(testItem);

    if ((testItem&TEST_RESISTANCE) != TEST_RESISTANCE)
        m_pResistanceCheck->setCheckState(Qt::Unchecked);
    if ((testItem&TEST_EXCITATION) != TEST_EXCITATION)
        m_pExcitationCheck->setCheckState(Qt::Unchecked);
    if ((testItem&TEST_RATIO) != TEST_RATIO)
        m_pChangeRateCheck->setCheckState(Qt::Unchecked);
    if ((testItem&TEST_BURDEN) != TEST_BURDEN)
        m_pLoadCheck->setCheckState(Qt::Unchecked);

    if (testItem&TEST_RESISTANCE)
        m_pResistanceCheck->setCheckState(Qt::Checked);
    if (testItem&TEST_EXCITATION)
        m_pExcitationCheck->setCheckState(Qt::Checked);
    if (testItem&TEST_RATIO)
        m_pChangeRateCheck->setCheckState(Qt::Checked);
    if (testItem&TEST_BURDEN)
        m_pLoadCheck->setCheckState(Qt::Checked);
}

void CTestItemWidget::initWidget()
{
    m_pResistanceCheck = new MQCheckBox(tr("电阻"));
    m_pExcitationCheck = new MQCheckBox(tr("励磁"));
    m_pChangeRateCheck = new MQCheckBox(tr("变比"));
    m_pLoadCheck       = new MQCheckBox(tr("负荷"));

    QHBoxLayout *groupLayout = new QHBoxLayout;
    groupLayout->setContentsMargins(15,15,15,0);
    groupLayout->addWidget(m_pResistanceCheck);
    groupLayout->addWidget(m_pExcitationCheck);
    groupLayout->addWidget(m_pChangeRateCheck);
    groupLayout->addWidget(m_pLoadCheck);

    this->setLayout(groupLayout);

    connect(m_pResistanceCheck,SIGNAL(stateChanged(int)),this,SLOT(onCheckBoxClicked(int)));
    connect(m_pExcitationCheck,SIGNAL(stateChanged(int)),this,SLOT(onCheckBoxClicked(int)));
    connect(m_pChangeRateCheck,SIGNAL(stateChanged(int)),this,SLOT(onCheckBoxClicked(int)));
    connect(m_pLoadCheck,      SIGNAL(stateChanged(int)),this,SLOT(onCheckBoxClicked(int)));
}

void CTestItemWidget::onCurrentType()
{
    m_pLoadCheck->show();
}

void CTestItemWidget::onVoltageType()
{
    m_pLoadCheck->hide();
}

void CTestItemWidget::setDefaultValue()
{
    if(TEST_RESISTANCE == CDataManager::getInstance()->getTestItem())
    {
        m_pResistanceCheck->setCheckState(Qt::Checked);
    }
    else if(TEST_EXCITATION == CDataManager::getInstance()->getTestItem())
    {
        m_pExcitationCheck->setCheckState(Qt::Checked);
    }
    else if(TEST_RATIO == CDataManager::getInstance()->getTestItem())
    {
        m_pChangeRateCheck->setCheckState(Qt::Checked);
    }
    else if(TEST_BURDEN == CDataManager::getInstance()->getTestItem())
    {
        m_pLoadCheck->setCheckState(Qt::Checked);
    }
}

void CTestItemWidget::addEventFilter()
{
    m_pResistanceCheck->installEventFilter(this);
    m_pExcitationCheck->installEventFilter(this);
    m_pChangeRateCheck->installEventFilter(this);
    m_pLoadCheck->installEventFilter(this);
}
