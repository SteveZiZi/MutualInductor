#include "mqtabwidget.h"
#include <QDebug>
MQTabWidget::MQTabWidget(QWidget *parent):QTabWidget(parent)
{

}
/**********************************************************
 * *@brief  捕获ESC和ENTER事件，做一些必要的处理
 * *@author  qinghuasun
 * *@para    QKeyEvent
 * *@return  void
 * *@data    2016/5/31
 * *******************************************************/
void MQTabWidget::keyPressEvent(QKeyEvent *eve)
{
    QWidget *curwidget=NULL;
    int eventvalue=eve->key();
    switch (eventvalue) {
    case Qt::Key_Return:
    case Qt::Key_Down:
        curwidget=this->currentWidget();//当前的窗口
        if(curwidget!=NULL)
        {
          this->clearFocus();
          curwidget->setFocus();
          //curwidget->nextInFocusChain()->setFocus();
          eve->accept();
        }
        break;
    case Qt::Key_Escape:
        curwidget=this->currentWidget();//当前的窗口
        if(curwidget!=NULL)
        {
            if(curwidget->focusWidget()!=NULL)//首先判断是否有焦点，没有说明焦点已经在TabWidget上，不动作
            {
                curwidget->focusWidget()->clearFocus();
                curwidget->clearFocus();
                this->setFocus();
                eve->accept();
            }
        }
        break;
    default:
        QTabWidget::keyPressEvent(eve);
        break;
    }
}
