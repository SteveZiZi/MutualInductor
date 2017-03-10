#include "mutualInductorTypeWidget.h"

#include <QHBoxLayout>
#include <QEvent>
#include <QKeyEvent>

#include "src/CTTester/dataManager/dataManager.h"
#include "src/CTTester/overrideControl/mqcheckbox.h"


CMutualInductorTypeWidget::CMutualInductorTypeWidget(QWidget *parent) : QGroupBox(parent)
{
    this->setTitle(tr("互感器类型"));

    initWidget();
    setDefauleValue();
    addEventFilter();
}

int CMutualInductorTypeWidget::getInductorType()
{
    if (m_pCurrentType->isChecked()) {
        return 0;
    }
    else {
        return 1;
    }
}

void CMutualInductorTypeWidget::focusInEvent(QFocusEvent * event)
{
    focusNextChild();

    return QWidget::focusInEvent(event);
}

bool CMutualInductorTypeWidget::eventFilter(QObject *obj, QEvent *eve)
{
    if(m_pCurrentType == obj || m_pVoltageType == obj)
    {
        if(eve->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(eve);
            switch (keyEvent->key())
            {
            case Qt::Key_Up:
                if(m_pCurrentType == obj)
                    focusPreviousChild();
                else
                    this->parentWidget()->eventFilter(this,eve);
                return true;
            case Qt::Key_Down:
                this->parentWidget()->eventFilter(this,eve);
                return true;
            case Qt::Key_Left:
                focusPreviousChild();
                return true;
            case Qt::Key_Right:
                focusNextChild();
                return true;
            default:
                break;
            }
        }
    }

    return false;
}

void CMutualInductorTypeWidget::onCheckBoxClicked(int status)
{
    if (Qt::Checked == status) {
        if (m_pCurrentType == sender()){
            m_pVoltageType->setChecked(false);
        }
        else{
            m_pCurrentType->setChecked(false);
        }
    }
    else if (Qt::Unchecked == status) {
        if (m_pCurrentType == sender()){
            m_pVoltageType->setChecked(true);
        }
        else{
            m_pCurrentType->setChecked(true);
        }
    }

    int type = 0;
    if(m_pCurrentType->isChecked())
        CDataManager::getInstance()->setInductorType(INDUCTOR_CT);
    else{
        CDataManager::getInstance()->setInductorType(INDUCTOR_PT);
        type = 1;
    }

    emit inductorChanged(type);
}

void CMutualInductorTypeWidget::initWidget()
{
    m_pCurrentType = new MQCheckBox(tr("CT"));
    m_pVoltageType = new MQCheckBox(tr("PT"));
    m_pCurrentType->setEnabled(false);
    m_pVoltageType->setEnabled(false);


    QHBoxLayout *groupLayout = new QHBoxLayout;
    groupLayout->setContentsMargins(15,15,20,0);
    groupLayout->addWidget(m_pCurrentType);
    groupLayout->addWidget(m_pVoltageType);

    this->setLayout(groupLayout);

    connect(m_pCurrentType,SIGNAL(stateChanged(int)),this,SLOT(onCheckBoxClicked(int)));
    connect(m_pVoltageType,SIGNAL(stateChanged(int)),this,SLOT(onCheckBoxClicked(int)));
}

void CMutualInductorTypeWidget::setDefauleValue()
{
    if(INDUCTOR_CT == CDataManager::getInstance()->getInductorType())
    {
        m_pCurrentType->setChecked(true);
        m_pVoltageType->setChecked(false);
    }
    else if(INDUCTOR_PT == CDataManager::getInstance()->getInductorType())
    {
        m_pVoltageType->setChecked(true);
        m_pCurrentType->setChecked(false);
    }
}

void CMutualInductorTypeWidget::addEventFilter()
{
    m_pCurrentType->installEventFilter(this);
    m_pVoltageType->installEventFilter(this);
}

