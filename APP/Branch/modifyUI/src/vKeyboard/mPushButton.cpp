#include "mPushButton.h"

#include <QDebug>
#include <QTimer>
#include <QKeyEvent>
#include <QStylePainter>

MPushButton::MPushButton(QWidget* parent) 
: QPushButton(parent) 
, m_bPressed(false)
, m_priorityDrawStatus(FOCUSED_PRIORITY)
, m_bSplash(false)
, m_bSplashing(false)
{

}

MPushButton::~MPushButton()
{

}

void MPushButton::SetPressMode(bool bPressed)
{
    if (m_bPressed != bPressed) {
        m_bPressed = bPressed;
    }
    this->repaint();
}

void MPushButton::paintEvent(QPaintEvent */*e*/)
{
    QStylePainter p(this);
    QStyleOptionButton option;
    initStyleOption(&option);

    if (m_priorityDrawStatus == PRESSED_PRIORITY) {
        if (m_bPressed) {
            option.state &= ~QStyle::State_Raised;
            option.state |= QStyle::State_Sunken;
        }
    }
    else {
        if (m_bSplash) {
            if (!m_bSplashing) {
                m_bSplashing = true;
                QTimer::singleShot(200, this, SLOT(finishSplash()));
            }
            option.state &= ~QStyle::State_Raised;
            option.state |= QStyle::State_Sunken;
        }
        else {
            if (!(option.state&QStyle::State_HasFocus)) {
                if (m_bPressed) {
                    option.state &= ~QStyle::State_Raised;
                    option.state |= QStyle::State_Sunken;
                }
            }
        }
    }

    p.drawControl(QStyle::CE_PushButton, option);    
}

void MPushButton::keyPressEvent(QKeyEvent* event)
{
    //qDebug() << "MPushButton::keyPressEvent" << hex << event->key();

    switch(event->key()) 
    {
    case Qt::Key_Enter:
    case Qt::Key_Return:
    {
        click();
        if (priorityDrawStatus() != PRESSED_PRIORITY)
            splash();
        event->accept();
        break;
    }
    default:
        event->ignore();
        break;
    }
}

void MPushButton::splash(void)
{
    m_bSplash = true;
    m_bSplashing = false;
    repaint();
}

void MPushButton::finishSplash(void)
{
    m_bSplash = false;
    m_bSplashing = false;
    if (m_priorityDrawStatus == SPLASHED_LOST_FOCUS_SET_PRESSED) {
        SetPressMode(!pressMode());
    }
    else {
        repaint();
    }
}
