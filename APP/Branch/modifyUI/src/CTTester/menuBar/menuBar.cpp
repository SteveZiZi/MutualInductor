#include "menuBar.h"

#include <QKeyEvent>
#include <QSignalMapper>
#include <QDebug>
#include <QApplication>

#include "src/CTTester/common.h"
#include "src/CTTester/overrideControl/mPushButton.h"

#define     BUTTON_WIDTH        MENU_BAR_W-2
#define     BUTTON_HEIGHT       60


CMenuBar::CMenuBar(QWidget *parent) : QGroupBox(parent)
    , m_defaultWidth(BUTTON_WIDTH)
    , m_defaultHeight(BUTTON_HEIGHT)
{
    m_signalMapper = new QSignalMapper(this);
    connect(m_signalMapper, SIGNAL(mapped(QObject*)), this, SLOT(menuClicked(QObject*)));
}

CMenuBar::~CMenuBar()
{

}

void CMenuBar::updateActions(QList<QAction *> actions)
{
    removeAllSignalMappers();

    m_actions = actions;
    int btnSize = m_btns.size();
    int actionSize = m_actions.size();

    if (btnSize < actionSize) {
        int n = actionSize - btnSize;
        for(int i = 0; i < n; i++)
            addNewBtn();
    }
    else if (actionSize < btnSize) {
        int n = btnSize - actionSize;
        for(int i = 0; i < n; i++) {
            MPushButton* btn = m_btns.takeLast();
            if (btn)
                btn->deleteLater();
        }
    }

    for(int i = 0; i < actionSize; i++) {
        m_btns[i]->setEnabled(m_actions[i]->isEnabled());
        m_btns[i]->setText(m_actions[i]->text());
        m_signalMapper->setMapping(m_btns[i], m_actions[i]);
    }
}

void CMenuBar::changeActionsStatus()
{
    int btnSize = m_btns.size();
    int actionSize = m_actions.size();
    int size = qMin<int>(btnSize, actionSize);

    for(int i = 0; i < size; i++) {
        m_btns[i]->setEnabled(!m_btns[i]->isEnabled());
    }
}

void CMenuBar::changeActionsStatus(int index)
{
    Q_ASSERT(index >= 0 && index < m_btns.size());

    m_btns[index]->setEnabled(!m_btns[index]->isEnabled());
}

void CMenuBar::focusInEvent(QFocusEvent *event)
{
    return QWidget::focusInEvent(event);
}

void CMenuBar::keyPressEvent(QKeyEvent * event)
{
    switch(event->key()) {
    case Qt::Key_Escape:
    case Qt::Key_Left:{
        QKeyEvent *e = new QKeyEvent(event->type(), event->key(), event->modifiers());
        QApplication::postEvent(this->parent(), e);
        event->accept();
        break;
    }
    case Qt::Key_Right:
        event->accept();
        break;
    default:
        QGroupBox::keyPressEvent(event);
        break;
    }
}

void CMenuBar::menuClicked(QObject *obj)
{
    QAction* action = dynamic_cast<QAction*>(obj);
    if (action)
        emit action->triggered(true);
}

void CMenuBar::addNewBtn()
{
    MPushButton *btn = new MPushButton(this);
    btn->resize(m_defaultWidth, m_defaultHeight);

    int y = 1;
    for(QList<MPushButton*>::iterator it = m_btns.begin(); it != m_btns.end(); it++)
        y += (*it)->height();
    btn->move(1, y);
    btn->show();
    connect(btn, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
    m_btns.append(btn);
}

void CMenuBar::removeAllSignalMappers()
{
    for(QList<MPushButton*>::iterator it = m_btns.begin(); it != m_btns.end(); it++)
        m_signalMapper->removeMappings(*it);
}

CMenuBar *GetMenuBar()
{
    static CMenuBar* menu = new CMenuBar();
    return menu;
}
