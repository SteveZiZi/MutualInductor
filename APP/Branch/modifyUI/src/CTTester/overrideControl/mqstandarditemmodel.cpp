#include "mqstandarditemmodel.h"

MQStandardItemModel::MQStandardItemModel(QObject *parent)
    :QStandardItemModel(parent)
{

}

QVariant MQStandardItemModel::data(const QModelIndex &item, int role) const
{
    QVariant value = QStandardItemModel::data(item, role);
    if (role == Qt::TextAlignmentRole)
    {
        value = (Qt::AlignCenter);
        return value;
    }
    return value;
}

Qt::ItemFlags MQStandardItemModel::flags(const QModelIndex &index) const
{
    if (index.column() < 1)
    {
       return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    else
    {
       return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
}

