#include <QDebug>
#include <QWidget>
#include <QKeyEvent>
#include <QLineEdit>
#include <QDesktopWidget>
#include <QFile>
#include <QRegExp>
#include "mqapplication.h"

#define CT_TEST_WIDGET_NAME     "ctTesterWidgetWindow"
#define VKEYBOARD_WIDGET_NAME   "vKeyboardClassWindow"
#define CHARACTER_PAD_NAME      "characterPad"
#define NUM_PAD_NAME            "numPad"


MQApplication::MQApplication(int &argc, char **argv)
    :QApplication(argc,argv)
{
}

bool MQApplication::notify(QObject *obj, QEvent *e)
{

    bool rc = false;
    QEvent::Type type = e->type();

    switch (type) {
    case QEvent::KeyPress:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
        {
//            QWidget *widget = focusWidget();
//            if(widget)
//                qDebug() << "focusWidget:" << widget->objectName();
//            else
//                qDebug() << "focusWidget:" << widget;
            break;
        }
    case QEvent::MouseButtonDblClick:
        e->accept();
        rc = true;
        break;
    default:
        break;
    }

    if (!rc) {
        rc = QApplication::notify(obj,e);
    }

    // LineEdit被点击，则显示软键盘（模态窗口）
    if (type == QEvent::MouseButtonPress)
    {
        if (obj->inherits("QLineEdit"))
        {
            QLineEdit *lineEdit = dynamic_cast<QLineEdit*>(obj);
            if (lineEdit && lineEdit->hasFocus() && !m_keyboard.isInputEdit(lineEdit))
            {
                m_keyboard.setRegExp(lineEdit->validator());// 正则表达式
                m_keyboard.setAssociatedLineEdit(lineEdit);
                m_keyboard.setDisplayText(lineEdit->text());
                adjustDesktop();
                m_keyboard.exec();
            }
        }
    }

    return rc;
}

void MQApplication::adjustDesktop()
{
    QDesktopWidget* desktopWidget = QApplication::desktop();
    QRect screenRect = desktopWidget->screenGeometry();

    QLineEdit* associatedEdit = m_keyboard.associatedLineEdit();
    QPoint topP = associatedEdit->mapToGlobal(QPoint(0, 0));

    int y = screenRect.height() - m_keyboard.height();
    int x = (screenRect.width()-m_keyboard.width())/2;

    if (topP.y()>= y) {
        y = 0;
    }

    m_keyboard.move(x, y);
}
