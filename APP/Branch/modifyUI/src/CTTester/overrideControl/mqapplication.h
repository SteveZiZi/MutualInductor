#ifndef MQAPPLICATION_H
#define MQAPPLICATION_H

#include <QApplication>
#include <src/vKeyboard/vkeyboard.h>

class MQApplication : public QApplication
{
public:
    MQApplication(int &argc, char **argv);

protected:
    // 跟踪按键事件
    bool notify(QObject *obj, QEvent *e);

private:
    void adjustDesktop();
private:
    vKeyboard m_keyboard;

};

#endif // MQAPPLICATION_H
