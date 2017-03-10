#ifndef MQTABLEVIEW_H
#define MQTABLEVIEW_H

#include <QTableView>

class MQTableView : public QTableView
{
public:
    MQTableView(QWidget *parent = 0);

protected:
    void focusInEvent(QFocusEvent * event);
    void focusOutEvent(QFocusEvent * event);
};

#endif // MQTABLEVIEW_H
