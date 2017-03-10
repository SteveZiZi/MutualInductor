#include "nofocusItemdelegate.h"

NoFocusItemDelegate::NoFocusItemDelegate(QObject * parent)
    :QItemDelegate(parent)
{

}

void NoFocusItemDelegate::drawFocus(QPainter */*painter*/,
                                    const QStyleOptionViewItem &/*option*/,
                                    const QRect &/*rect*/) const
{

}

void NoFocusItemDelegate::drawCheck(QPainter */*painter*/,
                                    const QStyleOptionViewItem &/*option*/,
                                    const QRect &/*rect*/,
                                    Qt::CheckState /*state*/) const
{

}

