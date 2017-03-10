#ifndef MQMESSAGEBOX_H
#define MQMESSAGEBOX_H

#include <QMessageBox>

class MQMessageBox : public QMessageBox
{
public:
    MQMessageBox(QWidget *parent = 0);
    MQMessageBox(Icon icon, const QString &title, const QString &text,
                 StandardButtons buttons = NoButton, QWidget *parent = 0,
                 Qt::WindowFlags flags = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // MQMESSAGEBOX_H
