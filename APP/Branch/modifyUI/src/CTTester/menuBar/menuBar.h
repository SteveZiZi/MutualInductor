#ifndef MENUBAR_H
#define MENUBAR_H

#include <QGroupBox>
#include <QList>
#include <QAction>

class QKeyEvent;
class MPushButton;
class QSignalMapper;

class CMenuBar : public QGroupBox
{
    Q_OBJECT
public:
    explicit CMenuBar(QWidget *parent = 0);
    virtual ~CMenuBar();
//    void setAss

signals:
    void actionTriggered(QAction*);

public slots:
    void updateActions(QList<QAction*> actions);
    void changeActionsStatus();
    void changeActionsStatus(int index);

protected:
    void focusInEvent(QFocusEvent * event);
    void keyPressEvent(QKeyEvent *);

private slots:
    void menuClicked(QObject* obj);

private:
    void addNewBtn();
    void removeAllSignalMappers();

private:
    QList<QAction*> m_actions;
    QList<MPushButton*> m_btns;
    int m_defaultWidth;
    int m_defaultHeight;
    QSignalMapper *m_signalMapper;
};

CMenuBar* GetMenuBar();

#endif // MENUBAR_H
