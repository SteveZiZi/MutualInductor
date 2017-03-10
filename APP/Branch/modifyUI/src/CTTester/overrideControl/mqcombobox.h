#ifndef MQCOMBOBOX_H
#define MQCOMBOBOX_H
#include <QComboBox>
#include <QWidget>
#include <QKeyEvent>

class MQListView;
class MQComboBox : public QComboBox
{
public:
    MQComboBox(QWidget *parent=0);
protected:
    void keyPressEvent(QKeyEvent *);
};

#endif // MQCOMBOBOX_H
