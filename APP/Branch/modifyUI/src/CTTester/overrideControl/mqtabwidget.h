#ifndef MQTABWIDGET_H
#define MQTABWIDGET_H
#include <QTabWidget>
#include <QKeyEvent>
class MQTabWidget : public QTabWidget
{
public:
    MQTabWidget(QWidget *parent=0);
public:
    void keyPressEvent(QKeyEvent *);//处理一些事件
};

#endif // MQTABWIDGET_H
