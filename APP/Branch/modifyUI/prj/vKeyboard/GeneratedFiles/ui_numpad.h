/********************************************************************************
** Form generated from reading UI file 'numpad.ui'
**
** Created: Thu Dec 15 10:13:19 2016
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NUMPAD_H
#define UI_NUMPAD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>
#include <mPushButton.h>

QT_BEGIN_NAMESPACE

class Ui_numPad
{
public:
    MPushButton *pushButton_1;
    MPushButton *pushButton_2;
    MPushButton *pushButton_3;
    MPushButton *pushButton_4;
    MPushButton *pushButton_5;
    MPushButton *pushButton_6;
    MPushButton *pushButton_7;
    MPushButton *pushButton_8;
    MPushButton *pushButton_9;
    MPushButton *pushButton_0;
    MPushButton *pushButton_point;
    MPushButton *pushButton_Backspace;
    MPushButton *pushButton_Enter;
    MPushButton *pushButton_oem1;
    MPushButton *pushButton_plus;
    MPushButton *pushButton_minus;
    MPushButton *pushButton_oem2;
    MPushButton *pushButton_oem3;
    MPushButton *pushButton_multi;
    MPushButton *pushButton_oem4;
    MPushButton *pushButton_div;
    MPushButton *pushButton_space;
    MPushButton *pushButton_enPad;
    MPushButton *pushButton_cancel;

    void setupUi(QWidget *numPad)
    {
        if (numPad->objectName().isEmpty())
            numPad->setObjectName(QString::fromUtf8("numPad"));
        numPad->resize(800, 200);
        pushButton_1 = new MPushButton(numPad);
        pushButton_1->setObjectName(QString::fromUtf8("pushButton_1"));
        pushButton_1->setGeometry(QRect(320, 100, 75, 45));
        pushButton_2 = new MPushButton(numPad);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(400, 100, 75, 45));
        pushButton_3 = new MPushButton(numPad);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setGeometry(QRect(480, 100, 75, 45));
        pushButton_4 = new MPushButton(numPad);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        pushButton_4->setGeometry(QRect(320, 50, 75, 45));
        pushButton_5 = new MPushButton(numPad);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));
        pushButton_5->setGeometry(QRect(400, 50, 75, 45));
        pushButton_6 = new MPushButton(numPad);
        pushButton_6->setObjectName(QString::fromUtf8("pushButton_6"));
        pushButton_6->setGeometry(QRect(480, 50, 75, 45));
        pushButton_7 = new MPushButton(numPad);
        pushButton_7->setObjectName(QString::fromUtf8("pushButton_7"));
        pushButton_7->setGeometry(QRect(320, 0, 75, 45));
        pushButton_8 = new MPushButton(numPad);
        pushButton_8->setObjectName(QString::fromUtf8("pushButton_8"));
        pushButton_8->setGeometry(QRect(400, 0, 75, 45));
        pushButton_9 = new MPushButton(numPad);
        pushButton_9->setObjectName(QString::fromUtf8("pushButton_9"));
        pushButton_9->setGeometry(QRect(480, 0, 75, 45));
        pushButton_0 = new MPushButton(numPad);
        pushButton_0->setObjectName(QString::fromUtf8("pushButton_0"));
        pushButton_0->setGeometry(QRect(320, 150, 151, 45));
        pushButton_point = new MPushButton(numPad);
        pushButton_point->setObjectName(QString::fromUtf8("pushButton_point"));
        pushButton_point->setGeometry(QRect(480, 150, 75, 45));
        pushButton_Backspace = new MPushButton(numPad);
        pushButton_Backspace->setObjectName(QString::fromUtf8("pushButton_Backspace"));
        pushButton_Backspace->setGeometry(QRect(640, 0, 151, 51));
        pushButton_Enter = new MPushButton(numPad);
        pushButton_Enter->setObjectName(QString::fromUtf8("pushButton_Enter"));
        pushButton_Enter->setGeometry(QRect(640, 130, 151, 61));
        pushButton_oem1 = new MPushButton(numPad);
        pushButton_oem1->setObjectName(QString::fromUtf8("pushButton_oem1"));
        pushButton_oem1->setGeometry(QRect(16, 0, 75, 45));
        pushButton_plus = new MPushButton(numPad);
        pushButton_plus->setObjectName(QString::fromUtf8("pushButton_plus"));
        pushButton_plus->setGeometry(QRect(176, 0, 75, 45));
        pushButton_minus = new MPushButton(numPad);
        pushButton_minus->setObjectName(QString::fromUtf8("pushButton_minus"));
        pushButton_minus->setGeometry(QRect(176, 50, 75, 45));
        pushButton_oem2 = new MPushButton(numPad);
        pushButton_oem2->setObjectName(QString::fromUtf8("pushButton_oem2"));
        pushButton_oem2->setGeometry(QRect(16, 50, 75, 45));
        pushButton_oem3 = new MPushButton(numPad);
        pushButton_oem3->setObjectName(QString::fromUtf8("pushButton_oem3"));
        pushButton_oem3->setGeometry(QRect(16, 100, 75, 45));
        pushButton_multi = new MPushButton(numPad);
        pushButton_multi->setObjectName(QString::fromUtf8("pushButton_multi"));
        pushButton_multi->setGeometry(QRect(96, 0, 75, 45));
        pushButton_oem4 = new MPushButton(numPad);
        pushButton_oem4->setObjectName(QString::fromUtf8("pushButton_oem4"));
        pushButton_oem4->setGeometry(QRect(16, 150, 75, 45));
        pushButton_div = new MPushButton(numPad);
        pushButton_div->setObjectName(QString::fromUtf8("pushButton_div"));
        pushButton_div->setGeometry(QRect(96, 50, 75, 45));
        pushButton_space = new MPushButton(numPad);
        pushButton_space->setObjectName(QString::fromUtf8("pushButton_space"));
        pushButton_space->setGeometry(QRect(100, 100, 151, 45));
        pushButton_enPad = new MPushButton(numPad);
        pushButton_enPad->setObjectName(QString::fromUtf8("pushButton_enPad"));
        pushButton_enPad->setGeometry(QRect(100, 150, 151, 45));
        pushButton_cancel = new MPushButton(numPad);
        pushButton_cancel->setObjectName(QString::fromUtf8("pushButton_cancel"));
        pushButton_cancel->setGeometry(QRect(640, 60, 151, 61));

        retranslateUi(numPad);

        QMetaObject::connectSlotsByName(numPad);
    } // setupUi

    void retranslateUi(QWidget *numPad)
    {
        numPad->setWindowTitle(QApplication::translate("numPad", "numPad", 0, QApplication::UnicodeUTF8));
        pushButton_1->setText(QApplication::translate("numPad", "1", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("numPad", "2", 0, QApplication::UnicodeUTF8));
        pushButton_3->setText(QApplication::translate("numPad", "3", 0, QApplication::UnicodeUTF8));
        pushButton_4->setText(QApplication::translate("numPad", "4", 0, QApplication::UnicodeUTF8));
        pushButton_5->setText(QApplication::translate("numPad", "5", 0, QApplication::UnicodeUTF8));
        pushButton_6->setText(QApplication::translate("numPad", "6", 0, QApplication::UnicodeUTF8));
        pushButton_7->setText(QApplication::translate("numPad", "7", 0, QApplication::UnicodeUTF8));
        pushButton_8->setText(QApplication::translate("numPad", "8", 0, QApplication::UnicodeUTF8));
        pushButton_9->setText(QApplication::translate("numPad", "9", 0, QApplication::UnicodeUTF8));
        pushButton_0->setText(QApplication::translate("numPad", "0", 0, QApplication::UnicodeUTF8));
        pushButton_point->setText(QApplication::translate("numPad", ".", 0, QApplication::UnicodeUTF8));
        pushButton_Backspace->setText(QApplication::translate("numPad", "\345\210\240  \351\231\244", 0, QApplication::UnicodeUTF8));
        pushButton_Enter->setText(QApplication::translate("numPad", "\347\241\256  \350\256\244", 0, QApplication::UnicodeUTF8));
        pushButton_oem1->setText(QApplication::translate("numPad", "#", 0, QApplication::UnicodeUTF8));
        pushButton_plus->setText(QApplication::translate("numPad", "+", 0, QApplication::UnicodeUTF8));
        pushButton_minus->setText(QApplication::translate("numPad", "-", 0, QApplication::UnicodeUTF8));
        pushButton_oem2->setText(QApplication::translate("numPad", "$", 0, QApplication::UnicodeUTF8));
        pushButton_oem3->setText(QApplication::translate("numPad", "&&", 0, QApplication::UnicodeUTF8));
        pushButton_multi->setText(QApplication::translate("numPad", "*", 0, QApplication::UnicodeUTF8));
        pushButton_oem4->setText(QApplication::translate("numPad", "@", 0, QApplication::UnicodeUTF8));
        pushButton_div->setText(QApplication::translate("numPad", "/", 0, QApplication::UnicodeUTF8));
        pushButton_space->setText(QApplication::translate("numPad", "\347\251\272 \346\240\274", 0, QApplication::UnicodeUTF8));
        pushButton_enPad->setText(QApplication::translate("numPad", "\350\213\261\346\226\207\351\224\256\347\233\230", 0, QApplication::UnicodeUTF8));
        pushButton_cancel->setText(QApplication::translate("numPad", "\345\217\226  \346\266\210", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class numPad: public Ui_numPad {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NUMPAD_H
