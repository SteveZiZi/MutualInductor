/********************************************************************************
** Form generated from reading UI file 'ctTesterWidget.ui'
**
** Created: Thu Dec 15 10:13:41 2016
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CTTESTERWIDGET_H
#define UI_CTTESTERWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ctTesterWidget
{
public:

    void setupUi(QWidget *ctTesterWidget)
    {
        if (ctTesterWidget->objectName().isEmpty())
            ctTesterWidget->setObjectName(QString::fromUtf8("ctTesterWidget"));
        ctTesterWidget->resize(400, 300);

        retranslateUi(ctTesterWidget);

        QMetaObject::connectSlotsByName(ctTesterWidget);
    } // setupUi

    void retranslateUi(QWidget *ctTesterWidget)
    {
        ctTesterWidget->setWindowTitle(QApplication::translate("ctTesterWidget", "ctTesterWidget", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ctTesterWidget: public Ui_ctTesterWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CTTESTERWIDGET_H
