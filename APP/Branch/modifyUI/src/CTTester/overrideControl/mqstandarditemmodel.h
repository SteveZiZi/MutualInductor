#ifndef MQSTANDARDITEMMODEL_H
#define MQSTANDARDITEMMODEL_H

#include <QStandardItemModel>

class MQStandardItemModel : public QStandardItemModel
{
public:
    MQStandardItemModel(QObject *parent = 0);

protected:
    /*
        brief: 设置模型的居中对齐，继承自QStandardItemModel
        param [in] 无
        param [out] 无
        return void
        author:MSZ
        date: 2015-08-24
    */
    QVariant data(const QModelIndex & item, int role) const;
    /*
        brief: 设置第一列不可选中，继承自QSqlTableModel
        param [in] 无
        param [out] 无
        return void
        author:MSZ
        date: 2015-08-24
    */
    Qt::ItemFlags flags(const QModelIndex &index) const;
};

#endif // MQSTANDARDITEMMODEL_H
