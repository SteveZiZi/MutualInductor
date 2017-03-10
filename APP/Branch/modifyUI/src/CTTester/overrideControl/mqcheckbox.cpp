#include "mqcheckbox.h"

#define     MIN_CHECKBOX_HEIGHT     30
#define     MIN_CHECKBOX_WIDTH      70

MQCheckBox::MQCheckBox(QWidget *parent)
    :QCheckBox(parent)
{
    this->setMinimumHeight(MIN_CHECKBOX_HEIGHT);
    this->setMinimumWidth(MIN_CHECKBOX_WIDTH);
}
MQCheckBox::MQCheckBox(const QString &text, QWidget *parent)
    :QCheckBox(text,parent)
{
    this->setMinimumHeight(MIN_CHECKBOX_HEIGHT);
    this->setMinimumWidth(MIN_CHECKBOX_WIDTH);
}

void MQCheckBox::keyPressEvent(QKeyEvent *eve)
{
    int keyvalue=eve->key();
    switch(keyvalue)
    {
    case Qt::Key_Return:
        this->setChecked(!(this->checkState()));
        eve->accept();
        break;
    case Qt::Key_Right:
        this->parentWidget()->parentWidget()->eventFilter(parentWidget(),eve);
        eve->accept();
        break;
    case Qt::Key_Left:
        this->parentWidget()->parentWidget()->eventFilter(parentWidget(),eve);
        eve->accept();
        break;
    default:
        QCheckBox::keyPressEvent(eve);
        break;
    }
}
