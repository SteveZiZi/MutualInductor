#include "mqgroupbox.h"

MQGroupBox::MQGroupBox(QWidget*parent,const QString & style)
    :QGroupBox(parent)
{
    this->setStyleSheet("QGroupBox{"+style+"}");
}
MQGroupBox::MQGroupBox(const QString &title, QWidget *parent,const QString & style):
    QGroupBox(title,parent)
{
    this->setStyleSheet("QGroupBox{"+style+"}");
}
/****************************
 * *@设置QGroupBox 的sheetstyle
 * *************************/
void MQGroupBox::setGroupSheetStyle(const QString &style)
{
    this->setStyleSheet("QGroupBox{"+style+"}");
}
