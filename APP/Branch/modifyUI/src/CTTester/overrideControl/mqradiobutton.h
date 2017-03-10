#ifndef MQRADIOBUTTON_H
#define MQRADIOBUTTON_H

#include <QRadioButton>
#include <QKeyEvent>

class MQRadioButton : public QRadioButton
{
public:
    MQRadioButton(QWidget *parent=0);
    MQRadioButton(const QString &text, QWidget *parent=0);

protected:
    void keyPressEvent(QKeyEvent *eve);
};

#endif // MQRADIOBUTTON_H
