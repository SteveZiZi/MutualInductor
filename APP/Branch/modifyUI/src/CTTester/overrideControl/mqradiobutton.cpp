#include "mqradiobutton.h"

MQRadioButton::MQRadioButton(QWidget *parent)
    :QRadioButton(parent)
{

}


MQRadioButton::MQRadioButton(const QString &text, QWidget *parent)
    :QRadioButton(text,parent)
{

}

void MQRadioButton::keyPressEvent(QKeyEvent *eve)
{
    int keyvalue=eve->key();
    switch(keyvalue)
    {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        this->setChecked(!(this->isChecked()));
        eve->accept();
        break;
    case Qt::Key_Right:
        this->parentWidget()->parentWidget()->parentWidget()->eventFilter(parentWidget()->parentWidget(),eve);
        eve->accept();
        break;
    case Qt::Key_Left:
        this->parentWidget()->parentWidget()->parentWidget()->eventFilter(parentWidget()->parentWidget(),eve);
        eve->accept();
        break;
    default:
        QRadioButton::keyPressEvent(eve);
        break;
    }
}
