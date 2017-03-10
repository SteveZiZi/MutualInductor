#ifndef MQCHECKBOX_H
#define MQCHECKBOX_H
#include <QCheckBox>
#include <QKeyEvent>
class MQCheckBox : public QCheckBox
{
public:
    MQCheckBox(QWidget *parent=0);
    MQCheckBox(const QString &text, QWidget *parent=0);
protected:
    void keyPressEvent(QKeyEvent *e);
};

#endif // MQCHECKBOX_H
