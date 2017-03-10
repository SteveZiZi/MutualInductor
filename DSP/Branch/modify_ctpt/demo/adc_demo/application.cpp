#include "application.h"
#include <QLineEdit>
#include <QComboBox>
#include <QKeyEvent>
#include <QDebug>
#include <QDesktopWidget>

#define USER_INACTIVE_TIMER_INTERVAL      1000

Application::Application(int &argc, char **argv)
: QApplication(argc,argv)
{

}

bool Application::notify(QObject *obj, QEvent *e)
{
    bool rc = QApplication::notify(obj, e);

    bool bEditMode = false;
    QEvent::Type type = e->type();
    if (type == QEvent::MouseButtonPress) {
        bEditMode = true;
    }
    else if (type == QEvent::KeyPress) {
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(e);
        if (keyEvent) {
            if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
                bEditMode = true;
            }
        }
    }

    if (bEditMode)
    {
        if (obj->inherits("QLineEdit") && focusWidget() == obj)
        {
            QLineEdit *lineEdit = dynamic_cast<QLineEdit*>(obj);
            if (lineEdit && !lineEdit->isReadOnly()) {
                QLineEdit* associatedEdit = m_keyboard.associatedLineEdit();
                if (associatedEdit && associatedEdit != lineEdit) {
                    m_keyboard.commit(true);
                }
                lineEdit->selectAll();
                m_keyboard.setAssociatedLineEdit(lineEdit);
                m_keyboard.setDisplayText(lineEdit->text());
                adjustDesktop();
                m_keyboard.show();
            }
        }
    }

    return rc;
}

void Application::adjustDesktop()
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
