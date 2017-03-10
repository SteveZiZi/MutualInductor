#include <QDebug>
#include <QApplication>
#include <QAbstractItemView>
#include "mqcombobox.h"

MQComboBox::MQComboBox(QWidget* parent):
    QComboBox(parent)
{

}
/********************************
 * 函数：KeyPressEvent
 * 目的：重写此函数，当按下键盘的回车键，弹出列表
 *
 * *******************************/
void MQComboBox::keyPressEvent(QKeyEvent *eve)
{
    int keyvalue=eve->key();
    if(eve->type()==QEvent::KeyPress)
    {
        switch(keyvalue)
        {
            case Qt::Key_Return:
            case Qt::Key_Enter:
                showPopup();
                eve->accept();
                break;
            case Qt::Key_Up:
                focusPreviousChild();
                eve->accept();
                break;
            case Qt::Key_Down:
                focusNextChild();
                eve->accept();
                break;
            default:
                QComboBox::keyPressEvent(eve);
                break;
        }
    }
}
