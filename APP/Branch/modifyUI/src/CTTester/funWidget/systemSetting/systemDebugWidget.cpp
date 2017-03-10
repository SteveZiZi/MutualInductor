#include "systemDebugWidget.h"

#include <QVBoxLayout>
#include <QKeyEvent>

#include "src/CTTester/overrideControl/mPushButton.h"

CSystemDebugWidget::CSystemDebugWidget(QWidget *parent) : QWidget(parent)
{
    initWidget();
    localConnect();
    addEventFilter();
}

void CSystemDebugWidget::focusInEvent(QFocusEvent *event)
{
    focusNextChild();

    return QWidget::focusInEvent(event);
}

bool CSystemDebugWidget::eventFilter(QObject *obj, QEvent *eve)
{
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(eve);
    if(m_pAdjustBtn == obj)
    {
        if(eve->type() == QEvent::KeyPress)
        {
            switch(keyEvent->key())
            {
            case Qt::Key_Up:
                focusPreviousChild();
                return true;
            case Qt::Key_Down:
                focusNextChild();
                return true;
            case Qt::Key_Right:
                this->parentWidget()->parentWidget()->eventFilter(this,eve);
                return true;
            default:
                break;
            }
        }
    }

    return false;
}

void CSystemDebugWidget::onBtnClicked()
{
#ifndef WIN32
    system("ts_calibrate");
#else

#endif
}

void CSystemDebugWidget::initWidget()
{
    m_pAdjustBtn = new MPushButton(tr("校准"));
    m_pAdjustBtn->setFixedSize(160,30);
    setMainLayout();
}

void CSystemDebugWidget::setMainLayout()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_pAdjustBtn);
    mainLayout->setAlignment(m_pAdjustBtn,Qt::AlignHCenter);

    this->setLayout(mainLayout);
}

void CSystemDebugWidget::localConnect()
{
    connect(m_pAdjustBtn,SIGNAL(clicked()),this,SLOT(onBtnClicked()));
}

void CSystemDebugWidget::addEventFilter()
{
    m_pAdjustBtn->installEventFilter(this);
}

