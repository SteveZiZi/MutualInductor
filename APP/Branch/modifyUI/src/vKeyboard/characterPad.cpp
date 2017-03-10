#include "characterPad.h"
#include <QSignalMapper>
#include <QApplication>
#include <QKeyEvent>
#include <QLineEdit>
#include <QTimer>
#include <QMouseEvent>


enum {
    KEY_Q,
    KEY_W,
    KEY_E,
    KEY_R,
    KEY_T,
    KEY_Y,
    KEY_U,
    KEY_I,
    KEY_O,
    KEY_P,

    KEY_A,
    KEY_S,
    KEY_D,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_J,
    KEY_K,
    KEY_L,

    KEY_CAPS,
    KEY_Z,
    KEY_X,
    KEY_C,
    KEY_V,
    KEY_B,
    KEY_N,
    KEY_M,
    KEY_BACKSPACE,

    KEY_Shift,
    KEY_NUM_PAD,
    KEY_SPACE,
    KEY_Cancel,
    KEY_ENTER,
};

struct KeyPadInfo
{
    int item;
    int key;
    QChar caption;
    QChar capsCaption;
};

const KeyPadInfo s_keyPadInfo[CCharacterPad::KEY_NUM] =
{
    { 0, Qt::Key_Q, 'q', 'Q' },
    { 1, Qt::Key_W, 'w', 'W' },
    { 2, Qt::Key_E, 'e', 'E' },
    { 3, Qt::Key_R, 'r', 'R' },
    { 4, Qt::Key_T, 't', 'T' },
    { 5, Qt::Key_Y, 'y', 'Y' },
    { 6, Qt::Key_U, 'u', 'U' },
    { 7, Qt::Key_I, 'i', 'I' },
    { 8, Qt::Key_O, 'o', 'O' },
    { 9, Qt::Key_P, 'p', 'P' },

    { 10, Qt::Key_A, 'a', 'A' },
    { 11, Qt::Key_S, 's', 'S' },
    { 12, Qt::Key_D, 'd', 'D' },
    { 13, Qt::Key_F, 'f', 'F' },
    { 14, Qt::Key_G, 'g', 'G' },
    { 15, Qt::Key_H, 'h', 'H' },
    { 16, Qt::Key_J, 'j', 'J' },
    { 17, Qt::Key_K, 'k', 'K' },
    { 18, Qt::Key_L, 'l', 'L' },

    { 19, Qt::Key_CapsLock, 0, 0 },
    { 20, Qt::Key_Z, 'z', 'Z' },
    { 21, Qt::Key_X, 'x', 'X' },
    { 22, Qt::Key_C, 'c', 'C' },
    { 23, Qt::Key_V, 'v', 'V' },
    { 24, Qt::Key_B, 'b', 'B' },
    { 25, Qt::Key_N, 'n', 'N' },
    { 26, Qt::Key_M, 'm', 'M' },
    { 27, Qt::Key_Backspace, 0, 0 },

    { 28, Qt::Key_Shift, 0, 0 },
    { 29, 0, 0, 0 },
    { 30, Qt::Key_Space, ' ', ' ' },
    { 31, Qt::Key_Cancel, 0, 0 },
    { 32, Qt::Key_Enter, 0, 0 }
};

CCharacterPad::CCharacterPad(QWidget* parent/* = 0*/, Qt::WindowFlags f/* = 0*/)
    : QWidget(parent, f)
    , m_bCaps(false)
    , m_bShift(false)
    , m_focusItem(KEY_ENTER)
{
    ui.setupUi(this);

    QSignalMapper *signalMapper = new QSignalMapper(this);

    Q_ASSERT(KEY_NUM == sizeof(s_keyPadInfo) / sizeof(s_keyPadInfo[0]));

    m_btn[KEY_Q] = ui.pushButton_Q;
    m_btn[KEY_W] = ui.pushButton_W;
    m_btn[KEY_E] = ui.pushButton_E;
    m_btn[KEY_R] = ui.pushButton_R;
    m_btn[KEY_T] = ui.pushButton_T;
    m_btn[KEY_Y] = ui.pushButton_Y;
    m_btn[KEY_U] = ui.pushButton_U;
    m_btn[KEY_I] = ui.pushButton_I;
    m_btn[KEY_O] = ui.pushButton_O;
    m_btn[KEY_P] = ui.pushButton_P;
    
    m_btn[KEY_A] = ui.pushButton_A;
    m_btn[KEY_S] = ui.pushButton_S;
    m_btn[KEY_D] = ui.pushButton_D;
    m_btn[KEY_F] = ui.pushButton_F;
    m_btn[KEY_G] = ui.pushButton_G;
    m_btn[KEY_H] = ui.pushButton_H;
    m_btn[KEY_J] = ui.pushButton_J;
    m_btn[KEY_K] = ui.pushButton_K;
    m_btn[KEY_L] = ui.pushButton_L;

    m_btn[KEY_CAPS] = ui.pushButton_Caps;
    m_btn[KEY_Z] = ui.pushButton_Z;
    m_btn[KEY_X] = ui.pushButton_X;
    m_btn[KEY_C] = ui.pushButton_C;
    m_btn[KEY_V] = ui.pushButton_V;
    m_btn[KEY_B] = ui.pushButton_B;
    m_btn[KEY_N] = ui.pushButton_N;
    m_btn[KEY_M] = ui.pushButton_M;
    m_btn[KEY_BACKSPACE] = ui.pushButton_Backspace;
    
    m_btn[KEY_Shift] = ui.pushButton_Shift;
    m_btn[KEY_NUM_PAD] = ui.pushButton_numPad;
    m_btn[KEY_SPACE] = ui.pushButton_Space;
    m_btn[KEY_Cancel] = ui.pushButton_cancel;
    m_btn[KEY_ENTER] = ui.pushButton_Enter;

    for (int i = 0; i < KEY_NUM; i++) {
        connect(m_btn[i], SIGNAL(clicked()), signalMapper, SLOT(map()));
        signalMapper->setMapping(m_btn[i], i);
        m_btn[i]->installEventFilter(this);
    }

    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(clickButton(int)));

    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(onTimerOut()));

    resetWidget();
}

CCharacterPad::~CCharacterPad()
{

}

void CCharacterPad::resetWidget(void)
{
    m_focusItem = KEY_ENTER;
    m_btn[m_focusItem]->setFocus();
}

bool CCharacterPad::eventFilter(QObject *watched, QEvent *event)
{
    int item = -1;
    for (int i = 0; i < KEY_NUM; i++) {
        if (m_btn[i] == watched) {
            item = i;
        }
    }

    if (item != -1) {
        if (event->type() == QEvent::FocusIn) {
            m_focusItem = item;
        }
        else if (event->type() == QEvent::KeyPress) {
            QKeyEvent *e = dynamic_cast<QKeyEvent*>(event);
            if (e) {
                if (dirKeyPress(e->key())) {
                    e->accept();
                    return true;
                }
            }
        }
        else if (event->type() == QEvent::MouseButtonPress){
            QMouseEvent *e = dynamic_cast<QMouseEvent*>(event);
            if (e) {
                if (item == KEY_BACKSPACE){
                    if(e->button() == Qt::LeftButton){
                        m_pTimer->start(1000);
                    }
                }
            }
        }
    }

    return QWidget::eventFilter(watched, event);     // 最后将事件交给上层对话框
}

void CCharacterPad::clickButton(int index)
{
    QKeyEvent *keyPressEvent = NULL;

    Q_ASSERT(index < KEY_NUM);

    if (s_keyPadInfo[index].caption != 0) {
        if (index != KEY_SPACE)
            keyPressEvent = new QKeyEvent(QEvent::KeyPress, s_keyPadInfo[index].key, Qt::NoModifier, m_btn[index]->text());
        else
            keyPressEvent = new QKeyEvent(QEvent::KeyPress, s_keyPadInfo[index].key, Qt::NoModifier, QString(' '));

        if (m_bShift) {
            if (m_bCaps) changeToUpper();
            else changeToLower();
            m_bShift = false;
        }
    }
    else {
        if (index == KEY_CAPS) {
            m_bCaps = !m_bCaps;
            m_bShift = false;
            if (m_bCaps) {
                changeToUpper();
                m_btn[index]->setText("CAPS");
            }
            else {
               changeToLower();
               m_btn[index]->setText("Caps");
            }
        }
        else if (index == KEY_Shift) {
            m_bShift = !m_bShift;
            if (m_bShift) {
                if (m_bCaps) changeToLower();
                else changeToUpper();
            }
            else {
                if (m_bCaps) changeToUpper();
                else changeToLower();
            }
        }
        else
        {
            if (m_bShift) {
                m_bShift = false;
                if (m_bCaps) changeToUpper();
                else changeToLower();
            }

            if (index == KEY_NUM_PAD) {
                emit changeIm(0);
            }
            else if (index == KEY_Cancel) {
                emit commit(false);
            }
            else if (index == KEY_ENTER) {
                emit commit(true);
            }
            else {
                if(index == KEY_BACKSPACE)
                    m_pTimer->stop();
                keyPressEvent = new QKeyEvent(QEvent::KeyPress, s_keyPadInfo[index].key, Qt::NoModifier);
            }
        }
    }

    if (keyPressEvent && m_inputLineEdit) {
        QApplication::postEvent(m_inputLineEdit, keyPressEvent);
    }
}

void CCharacterPad::onTimerOut()
{
    m_pTimer->setInterval(100);
    QKeyEvent *keyPressEvent = new QKeyEvent(QEvent::KeyPress, s_keyPadInfo[KEY_BACKSPACE].key, Qt::NoModifier);
    if (keyPressEvent && m_inputLineEdit) {
        QApplication::postEvent(m_inputLineEdit, keyPressEvent);
    }
}

void CCharacterPad::changeToLower()
{
    for (int i = 0; i < KEY_NUM; i++) {
        if (s_keyPadInfo[i].caption != 0) {
            m_btn[i]->setText(s_keyPadInfo[i].caption);
        }
    }
}

void CCharacterPad::changeToUpper()
{
    for (int i = 0; i < KEY_NUM; i++) {
        if (s_keyPadInfo[i].capsCaption != 0) {
            m_btn[i]->setText(s_keyPadInfo[i].capsCaption);
        }
    }
}

bool CCharacterPad::dirKeyPress(int dirKey)
{
    bool rc = true;
    switch (dirKey) {
    case Qt::Key_Left:
        if (m_focusItem > 0) {
            m_focusItem--;
        }
        else {
            m_focusItem = 0;
        }
        m_btn[m_focusItem]->setFocus();
        break;
    case Qt::Key_Right:
        m_focusItem++;
        if (m_focusItem >= KEY_NUM) {
            m_focusItem = KEY_NUM - 1;
        }
        m_btn[m_focusItem]->setFocus();
        break;
    case Qt::Key_Up:
        if (m_focusItem <= KEY_P) {

        }
        else if (m_focusItem <= KEY_L) {
            m_focusItem = m_focusItem - KEY_A + KEY_Q;
        }
        else if (m_focusItem <= KEY_BACKSPACE) {
            m_focusItem = m_focusItem - KEY_CAPS + KEY_A;
        }
        else {
            if (m_focusItem == KEY_Shift)
                m_focusItem = KEY_CAPS;
            else if (m_focusItem == KEY_NUM_PAD)
                m_focusItem = KEY_Z;
            else if (m_focusItem == KEY_SPACE)
                m_focusItem = KEY_V;
            else if (m_focusItem == KEY_Cancel)
                m_focusItem = KEY_N;
            else if (m_focusItem == KEY_ENTER)
                m_focusItem = KEY_BACKSPACE;
        }
        m_btn[m_focusItem]->setFocus();
        break;
    case Qt::Key_Down:
        if (m_focusItem <= KEY_P) {
            m_focusItem = m_focusItem - KEY_Q + KEY_A;
            if (m_focusItem > KEY_L)
                m_focusItem = KEY_L;
        }
        else if (m_focusItem <= KEY_L) {
            m_focusItem = m_focusItem - KEY_A + KEY_CAPS;
            if (m_focusItem > KEY_BACKSPACE)
                m_focusItem = KEY_BACKSPACE;
        }
        else if (m_focusItem <= KEY_BACKSPACE) {
            if (m_focusItem == KEY_CAPS)
                m_focusItem = KEY_Shift;
            else if (m_focusItem <= KEY_X)
                m_focusItem = KEY_NUM_PAD;
            else if (m_focusItem <= KEY_B)
                m_focusItem = KEY_SPACE;
            else if (m_focusItem <= KEY_M)
                m_focusItem = KEY_Cancel;
            else if (m_focusItem == KEY_BACKSPACE)
                m_focusItem = KEY_ENTER;
        }
        else {
        }
        m_btn[m_focusItem]->setFocus();
        break;
    default:
        rc = false;
        break;
    }

    return rc;
}
