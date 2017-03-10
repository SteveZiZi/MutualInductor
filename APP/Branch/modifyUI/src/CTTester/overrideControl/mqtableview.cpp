#include "mqtableview.h"

MQTableView::MQTableView(QWidget *parent)
    :QTableView(parent)
{

}

void MQTableView::focusInEvent(QFocusEvent *event)
{
    focusNextChild();

    return QTableView::focusInEvent(event);
}

void MQTableView::focusOutEvent(QFocusEvent *event)
{
    clearSelection();

    return QTableView::focusOutEvent(event);
}

