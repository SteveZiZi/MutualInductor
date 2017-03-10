#pragma once

#include <QListWidget>
#include <QtGui/QKeyEvent>
#include <QDebug>

class MListWidget : public QListWidget  
{
    Q_OBJECT
public:
    MListWidget(QWidget* parent = 0) : QListWidget(parent) {}
    virtual ~MListWidget() {}

protected:
    void keyPressEvent(QKeyEvent *event) {
        if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
            emit itemClicked(currentItem());
            event->accept();
        }
        else 
            QListWidget::keyPressEvent(event);
    }
};
