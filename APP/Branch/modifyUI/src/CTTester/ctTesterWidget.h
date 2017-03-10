#ifndef CTTESTERWIDGET_H
#define CTTESTERWIDGET_H

#include <QWidget>
#include "ui_ctTesterWidget.h"

namespace Ui {
class ctTesterWidget;
}

class QTabWidget;
class QTimer;
class QLabel;
class CFunTabWidget;
class CMenuBar;

class CCtTesterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CCtTesterWidget(QWidget *parent = 0);
    ~CCtTesterWidget();

protected:
    void keyPressEvent(QKeyEvent *e);

private slots:
    void clockTimerSlot();

private:
    void initWidget();

private:
    Ui::ctTesterWidget *ui;
    CFunTabWidget               *m_pTabWidget;
    QTimer                      *m_pClockTimer;
    QLabel                      *m_pClockLabel;
    CMenuBar                    *m_pMenuBar;

};

#endif // CTTESTERWIDGET_H
