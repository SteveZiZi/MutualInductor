/********************************************************************************
** Form generated from reading UI file 'characterPad.ui'
**
** Created: Thu Dec 15 10:13:19 2016
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHARACTERPAD_H
#define UI_CHARACTERPAD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>
#include <mPushButton.h>

QT_BEGIN_NAMESPACE

class Ui_characterPad
{
public:
    MPushButton *pushButton_Q;
    MPushButton *pushButton_W;
    MPushButton *pushButton_E;
    MPushButton *pushButton_R;
    MPushButton *pushButton_T;
    MPushButton *pushButton_Y;
    MPushButton *pushButton_U;
    MPushButton *pushButton_I;
    MPushButton *pushButton_O;
    MPushButton *pushButton_P;
    MPushButton *pushButton_A;
    MPushButton *pushButton_S;
    MPushButton *pushButton_D;
    MPushButton *pushButton_F;
    MPushButton *pushButton_G;
    MPushButton *pushButton_H;
    MPushButton *pushButton_J;
    MPushButton *pushButton_K;
    MPushButton *pushButton_L;
    MPushButton *pushButton_M;
    MPushButton *pushButton_Z;
    MPushButton *pushButton_X;
    MPushButton *pushButton_C;
    MPushButton *pushButton_V;
    MPushButton *pushButton_B;
    MPushButton *pushButton_N;
    MPushButton *pushButton_Space;
    MPushButton *pushButton_Enter;
    MPushButton *pushButton_cancel;
    MPushButton *pushButton_numPad;
    MPushButton *pushButton_Backspace;
    MPushButton *pushButton_Caps;
    MPushButton *pushButton_Shift;

    void setupUi(QWidget *characterPad)
    {
        if (characterPad->objectName().isEmpty())
            characterPad->setObjectName(QString::fromUtf8("characterPad"));
        characterPad->resize(800, 200);
        pushButton_Q = new MPushButton(characterPad);
        pushButton_Q->setObjectName(QString::fromUtf8("pushButton_Q"));
        pushButton_Q->setGeometry(QRect(0, 0, 75, 45));
        pushButton_W = new MPushButton(characterPad);
        pushButton_W->setObjectName(QString::fromUtf8("pushButton_W"));
        pushButton_W->setGeometry(QRect(80, 0, 75, 45));
        pushButton_E = new MPushButton(characterPad);
        pushButton_E->setObjectName(QString::fromUtf8("pushButton_E"));
        pushButton_E->setGeometry(QRect(160, 0, 75, 45));
        pushButton_R = new MPushButton(characterPad);
        pushButton_R->setObjectName(QString::fromUtf8("pushButton_R"));
        pushButton_R->setGeometry(QRect(240, 0, 75, 45));
        pushButton_T = new MPushButton(characterPad);
        pushButton_T->setObjectName(QString::fromUtf8("pushButton_T"));
        pushButton_T->setGeometry(QRect(320, 0, 75, 45));
        pushButton_Y = new MPushButton(characterPad);
        pushButton_Y->setObjectName(QString::fromUtf8("pushButton_Y"));
        pushButton_Y->setGeometry(QRect(400, 0, 75, 45));
        pushButton_U = new MPushButton(characterPad);
        pushButton_U->setObjectName(QString::fromUtf8("pushButton_U"));
        pushButton_U->setGeometry(QRect(480, 0, 75, 45));
        pushButton_I = new MPushButton(characterPad);
        pushButton_I->setObjectName(QString::fromUtf8("pushButton_I"));
        pushButton_I->setGeometry(QRect(560, 0, 75, 45));
        pushButton_O = new MPushButton(characterPad);
        pushButton_O->setObjectName(QString::fromUtf8("pushButton_O"));
        pushButton_O->setGeometry(QRect(640, 0, 75, 45));
        pushButton_P = new MPushButton(characterPad);
        pushButton_P->setObjectName(QString::fromUtf8("pushButton_P"));
        pushButton_P->setGeometry(QRect(720, 0, 75, 45));
        pushButton_A = new MPushButton(characterPad);
        pushButton_A->setObjectName(QString::fromUtf8("pushButton_A"));
        pushButton_A->setGeometry(QRect(40, 50, 75, 45));
        pushButton_S = new MPushButton(characterPad);
        pushButton_S->setObjectName(QString::fromUtf8("pushButton_S"));
        pushButton_S->setGeometry(QRect(120, 50, 75, 45));
        pushButton_D = new MPushButton(characterPad);
        pushButton_D->setObjectName(QString::fromUtf8("pushButton_D"));
        pushButton_D->setGeometry(QRect(200, 50, 75, 45));
        pushButton_F = new MPushButton(characterPad);
        pushButton_F->setObjectName(QString::fromUtf8("pushButton_F"));
        pushButton_F->setGeometry(QRect(280, 50, 75, 45));
        pushButton_G = new MPushButton(characterPad);
        pushButton_G->setObjectName(QString::fromUtf8("pushButton_G"));
        pushButton_G->setGeometry(QRect(360, 50, 75, 45));
        pushButton_H = new MPushButton(characterPad);
        pushButton_H->setObjectName(QString::fromUtf8("pushButton_H"));
        pushButton_H->setGeometry(QRect(440, 50, 75, 45));
        pushButton_J = new MPushButton(characterPad);
        pushButton_J->setObjectName(QString::fromUtf8("pushButton_J"));
        pushButton_J->setGeometry(QRect(520, 50, 75, 45));
        pushButton_K = new MPushButton(characterPad);
        pushButton_K->setObjectName(QString::fromUtf8("pushButton_K"));
        pushButton_K->setGeometry(QRect(600, 50, 75, 45));
        pushButton_L = new MPushButton(characterPad);
        pushButton_L->setObjectName(QString::fromUtf8("pushButton_L"));
        pushButton_L->setGeometry(QRect(680, 50, 75, 45));
        pushButton_M = new MPushButton(characterPad);
        pushButton_M->setObjectName(QString::fromUtf8("pushButton_M"));
        pushButton_M->setGeometry(QRect(590, 100, 75, 45));
        pushButton_Z = new MPushButton(characterPad);
        pushButton_Z->setObjectName(QString::fromUtf8("pushButton_Z"));
        pushButton_Z->setGeometry(QRect(110, 100, 75, 45));
        pushButton_X = new MPushButton(characterPad);
        pushButton_X->setObjectName(QString::fromUtf8("pushButton_X"));
        pushButton_X->setGeometry(QRect(190, 100, 75, 45));
        pushButton_C = new MPushButton(characterPad);
        pushButton_C->setObjectName(QString::fromUtf8("pushButton_C"));
        pushButton_C->setGeometry(QRect(270, 100, 75, 45));
        pushButton_V = new MPushButton(characterPad);
        pushButton_V->setObjectName(QString::fromUtf8("pushButton_V"));
        pushButton_V->setGeometry(QRect(350, 100, 75, 45));
        pushButton_B = new MPushButton(characterPad);
        pushButton_B->setObjectName(QString::fromUtf8("pushButton_B"));
        pushButton_B->setGeometry(QRect(430, 100, 75, 45));
        pushButton_N = new MPushButton(characterPad);
        pushButton_N->setObjectName(QString::fromUtf8("pushButton_N"));
        pushButton_N->setGeometry(QRect(510, 100, 75, 45));
        pushButton_Space = new MPushButton(characterPad);
        pushButton_Space->setObjectName(QString::fromUtf8("pushButton_Space"));
        pushButton_Space->setGeometry(QRect(250, 150, 271, 45));
        pushButton_Enter = new MPushButton(characterPad);
        pushButton_Enter->setObjectName(QString::fromUtf8("pushButton_Enter"));
        pushButton_Enter->setGeometry(QRect(654, 150, 141, 45));
        pushButton_cancel = new MPushButton(characterPad);
        pushButton_cancel->setObjectName(QString::fromUtf8("pushButton_cancel"));
        pushButton_cancel->setGeometry(QRect(540, 150, 91, 45));
        pushButton_numPad = new MPushButton(characterPad);
        pushButton_numPad->setObjectName(QString::fromUtf8("pushButton_numPad"));
        pushButton_numPad->setGeometry(QRect(140, 150, 91, 45));
        pushButton_Backspace = new MPushButton(characterPad);
        pushButton_Backspace->setObjectName(QString::fromUtf8("pushButton_Backspace"));
        pushButton_Backspace->setGeometry(QRect(684, 100, 111, 45));
        pushButton_Caps = new MPushButton(characterPad);
        pushButton_Caps->setObjectName(QString::fromUtf8("pushButton_Caps"));
        pushButton_Caps->setGeometry(QRect(0, 100, 91, 45));
        pushButton_Shift = new MPushButton(characterPad);
        pushButton_Shift->setObjectName(QString::fromUtf8("pushButton_Shift"));
        pushButton_Shift->setGeometry(QRect(0, 150, 121, 45));

        retranslateUi(characterPad);

        QMetaObject::connectSlotsByName(characterPad);
    } // setupUi

    void retranslateUi(QWidget *characterPad)
    {
        characterPad->setWindowTitle(QApplication::translate("characterPad", "characterPad", 0, QApplication::UnicodeUTF8));
        pushButton_Q->setText(QApplication::translate("characterPad", "q", 0, QApplication::UnicodeUTF8));
        pushButton_W->setText(QApplication::translate("characterPad", "w", 0, QApplication::UnicodeUTF8));
        pushButton_E->setText(QApplication::translate("characterPad", "e", 0, QApplication::UnicodeUTF8));
        pushButton_R->setText(QApplication::translate("characterPad", "r", 0, QApplication::UnicodeUTF8));
        pushButton_T->setText(QApplication::translate("characterPad", "t", 0, QApplication::UnicodeUTF8));
        pushButton_Y->setText(QApplication::translate("characterPad", "y", 0, QApplication::UnicodeUTF8));
        pushButton_U->setText(QApplication::translate("characterPad", "u", 0, QApplication::UnicodeUTF8));
        pushButton_I->setText(QApplication::translate("characterPad", "i", 0, QApplication::UnicodeUTF8));
        pushButton_O->setText(QApplication::translate("characterPad", "o", 0, QApplication::UnicodeUTF8));
        pushButton_P->setText(QApplication::translate("characterPad", "p", 0, QApplication::UnicodeUTF8));
        pushButton_A->setText(QApplication::translate("characterPad", "a", 0, QApplication::UnicodeUTF8));
        pushButton_S->setText(QApplication::translate("characterPad", "s", 0, QApplication::UnicodeUTF8));
        pushButton_D->setText(QApplication::translate("characterPad", "d", 0, QApplication::UnicodeUTF8));
        pushButton_F->setText(QApplication::translate("characterPad", "f", 0, QApplication::UnicodeUTF8));
        pushButton_G->setText(QApplication::translate("characterPad", "g", 0, QApplication::UnicodeUTF8));
        pushButton_H->setText(QApplication::translate("characterPad", "h", 0, QApplication::UnicodeUTF8));
        pushButton_J->setText(QApplication::translate("characterPad", "j", 0, QApplication::UnicodeUTF8));
        pushButton_K->setText(QApplication::translate("characterPad", "k", 0, QApplication::UnicodeUTF8));
        pushButton_L->setText(QApplication::translate("characterPad", "l", 0, QApplication::UnicodeUTF8));
        pushButton_M->setText(QApplication::translate("characterPad", "m", 0, QApplication::UnicodeUTF8));
        pushButton_Z->setText(QApplication::translate("characterPad", "z", 0, QApplication::UnicodeUTF8));
        pushButton_X->setText(QApplication::translate("characterPad", "x", 0, QApplication::UnicodeUTF8));
        pushButton_C->setText(QApplication::translate("characterPad", "c", 0, QApplication::UnicodeUTF8));
        pushButton_V->setText(QApplication::translate("characterPad", "v", 0, QApplication::UnicodeUTF8));
        pushButton_B->setText(QApplication::translate("characterPad", "b", 0, QApplication::UnicodeUTF8));
        pushButton_N->setText(QApplication::translate("characterPad", "n", 0, QApplication::UnicodeUTF8));
        pushButton_Space->setText(QApplication::translate("characterPad", "\347\251\272  \346\240\274", 0, QApplication::UnicodeUTF8));
        pushButton_Enter->setText(QApplication::translate("characterPad", "\347\241\256  \350\256\244", 0, QApplication::UnicodeUTF8));
        pushButton_cancel->setText(QApplication::translate("characterPad", "\345\217\226  \346\266\210", 0, QApplication::UnicodeUTF8));
        pushButton_numPad->setText(QApplication::translate("characterPad", "\346\225\260\345\255\227\351\224\256\347\233\230", 0, QApplication::UnicodeUTF8));
        pushButton_Backspace->setText(QApplication::translate("characterPad", "\345\210\240  \351\231\244", 0, QApplication::UnicodeUTF8));
        pushButton_Caps->setText(QApplication::translate("characterPad", "Caps", 0, QApplication::UnicodeUTF8));
        pushButton_Shift->setText(QApplication::translate("characterPad", "Shift", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class characterPad: public Ui_characterPad {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHARACTERPAD_H
