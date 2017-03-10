#include "mLineEdit.h"
#include <QKeyEvent>
#include <QApplication>

MLineEdit::MLineEdit(QWidget *parent)
    : QLineEdit(parent)
{

}

MLineEdit::MLineEdit(const QString &text, QWidget *parent)
    : QLineEdit(text,parent)
{

}

void MLineEdit::keyPressEvent(QKeyEvent *eve)
{
    switch(eve->key())
    {
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_Down:
    case Qt::Key_Up:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Escape:{
        QKeyEvent *e = new QKeyEvent(eve->type(), eve->key(), eve->modifiers());
        QApplication::postEvent(this->parent(), e);
        eve->accept();
        break;
    }
    default:
        eve->ignore();
        break;
    }
}
