#ifndef MQGROUPBOX_H
#define MQGROUPBOX_H
#include <QGroupBox>
#define QGROUPBOX_DEFAULT_STYTLE  "border:1px solid rgb(20,30,30);border-style:outset;background-color:rgb(58,109,171);"
class MQGroupBox : public QGroupBox
{
public:
    MQGroupBox(QWidget* parent=0,const QString& style=QGROUPBOX_DEFAULT_STYTLE);
    MQGroupBox(const QString & title,QWidget* parent=0,const QString& style=QGROUPBOX_DEFAULT_STYTLE);
    void setGroupSheetStyle(const QString& style=QGROUPBOX_DEFAULT_STYTLE);//设置MQGroupBox的类型
};

#endif // MQGROUPBOX_H
