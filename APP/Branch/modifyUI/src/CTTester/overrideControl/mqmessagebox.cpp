#include "mqmessagebox.h"

MQMessageBox::MQMessageBox(QWidget *parent)
    :QMessageBox(parent)
{

}

MQMessageBox::MQMessageBox(QMessageBox::Icon icon, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QWidget *parent, Qt::WindowFlags flags)
    :QMessageBox(icon,title,text,buttons,parent,flags)
{

}

void MQMessageBox::resizeEvent(QResizeEvent *event)
{
    setFixedSize(250,180);
    QMessageBox::resizeEvent(event);
}

