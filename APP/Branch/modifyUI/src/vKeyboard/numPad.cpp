#include "numPad.h"
#include <QSignalMapper>
#include <QApplication>
#include <QKeyEvent>
#include <QLineEdit>
#include <QTimer>
#include <QMouseEvent>


enum {
    KEY_OEM_1,
    KEY_MULTI,
    KEY_PLUS,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_BACKSPACE,

    KEY_OEM_2,
    KEY_DIV,
    KEY_MINUS,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_CANCEL,

    KEY_OEM_3,
    KEY_SPACE,
    KEY_1,
    KEY_2,
    KEY_3,

    KEY_OEM_4,
    KEY_CHARACTER_PAD,
    KEY_0,
    KEY_POINT,
    KEY_ENTER
};

struct KeyPadInfo
{
    int item;
    int key;
    QChar caption;
};

const KeyPadInfo s_keyPadInfo[CNumPad::KEY_NUM] =
{
    { 0, Qt::Key_unknown,   '#' },
    { 1, Qt::Key_Asterisk,  '*' },
    { 2, Qt::Key_Plus,      '+' },
    { 3, Qt::Key_7,         '7' },
    { 4, Qt::Key_8,         '8' },
    { 5, Qt::Key_9,         '9' },
    { 6, Qt::Key_Backspace,  0  },

    { 7, Qt::Key_Dollar, '$' },
    { 8, Qt::Key_unknown, '/' },
    { 9, Qt::Key_Minus, '-' },
    { 10, Qt::Key_4, '4' },
    { 11, Qt::Key_5, '5' },
    { 12, Qt::Key_4, '6' },
    { 13, Qt::Key_Escape, 0 },

    { 14, Qt::Key_unknown, '&' },
    { 15, Qt::Key_Space, ' ' },
    { 16, Qt::Key_1, '1' },
    { 17, Qt::Key_2, '2' },
    { 18, Qt::Key_3, '3' },

    { 19, Qt::Key_unknown, '@' },
    { 20, 0, 0 },
    { 21, Qt::Key_0,     '0' },
    { 22, Qt::Key_NumberSign, '.' },
    { 23, Qt::Key_Enter, 0 }
};


CNumPad::CNumPad(QWidget* parent/* = 0*/, Qt::WindowFlags f/* = 0*/)
    : QWidget(parent, f)
    , m_focusItem(KEY_ENTER)
{
    ui.setupUi(this);

    QSignalMapper *signalMapper = new QSignalMapper(this);

    Q_ASSERT(KEY_NUM == sizeof(s_keyPadInfo) / sizeof(s_keyPadInfo[0]));

    m_btn[KEY_OEM_1] = ui.pushButton_oem1;
    m_btn[KEY_MULTI] = ui.pushButton_multi;
    m_btn[KEY_PLUS] = ui.pushButton_plus;
    m_btn[KEY_7] = ui.pushButton_7;
    m_btn[KEY_8] = ui.pushButton_8;
    m_btn[KEY_9] = ui.pushButton_9;
    m_btn[KEY_BACKSPACE] = ui.pushButton_Backspace;

    m_btn[KEY_OEM_2] = ui.pushButton_oem2;
    m_btn[KEY_DIV] = ui.pushButton_div;
    m_btn[KEY_MINUS] = ui.pushButton_minus;
    m_btn[KEY_4] = ui.pushButton_4;
    m_btn[KEY_5] = ui.pushButton_5;
    m_btn[KEY_6] = ui.pushButton_6;
    m_btn[KEY_CANCEL] = ui.pushButton_cancel;

    m_btn[KEY_OEM_3] = ui.pushButton_oem3;
    m_btn[KEY_SPACE] = ui.pushButton_space;
    m_btn[KEY_1] = ui.pushButton_1;
    m_btn[KEY_2] = ui.pushButton_2;
    m_btn[KEY_3] = ui.pushButton_3;
    m_btn[KEY_ENTER] = ui.pushButton_Enter;

    m_btn[KEY_OEM_4] = ui.pushButton_oem4;
    m_btn[KEY_CHARACTER_PAD] = ui.pushButton_enPad;
    m_btn[KEY_0] = ui.pushButton_0;
    m_btn[KEY_POINT] = ui.pushButton_point;

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

CNumPad::~CNumPad()
{

}

void CNumPad::resetWidget(void)
{
    m_focusItem = KEY_ENTER;
    m_btn[m_focusItem]->setFocus();
}

bool CNumPad::eventFilter(QObject *watched, QEvent *event)
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

void CNumPad::onTimerOut()
{
    m_pTimer->setInterval(100);
    QKeyEvent *keyPressEvent = new QKeyEvent(QEvent::KeyPress, s_keyPadInfo[KEY_BACKSPACE].key, Qt::NoModifier);
    if (keyPressEvent && m_inputLineEdit) {
        QApplication::postEvent(m_inputLineEdit, keyPressEvent);
    }
}

void CNumPad::clickButton(int index)
{
    QKeyEvent *keyPressEvent = NULL;

    Q_ASSERT(index < KEY_NUM);

    if (s_keyPadInfo[index].caption != 0) {
        keyPressEvent = new QKeyEvent(QEvent::KeyPress, s_keyPadInfo[index].key, Qt::NoModifier, s_keyPadInfo[index].caption);
    }
    else {
        if (index == KEY_CHARACTER_PAD) {
            emit changeIm(1);
        }
        else if (index == KEY_CANCEL) {
            resetWidget();
            emit commit(false);
        }
        else if (index == KEY_ENTER) {
            resetWidget();
            emit commit(true);
        }
        else {
            if(index == KEY_BACKSPACE)
                m_pTimer->stop();
            keyPressEvent = new QKeyEvent(QEvent::KeyPress, s_keyPadInfo[index].key, Qt::NoModifier);
        }
    }

    if (keyPressEvent && m_inputLineEdit) {
        QApplication::postEvent(m_inputLineEdit, keyPressEvent);
    }
}

bool CNumPad::dirKeyPress(int dirKey)
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
        if (m_focusItem <= KEY_BACKSPACE) {

        }
        else if (m_focusItem <= KEY_CANCEL) {
            m_focusItem = m_focusItem - KEY_OEM_2 + KEY_OEM_1;
        }
        else if (m_focusItem <= KEY_SPACE) {
            m_focusItem = m_focusItem - KEY_OEM_3 + KEY_OEM_2;
        }
        else if (m_focusItem <= KEY_3) {
            m_focusItem = m_focusItem - KEY_1 + KEY_4;
        }
        else if (m_focusItem <= KEY_0) {
            m_focusItem = m_focusItem - KEY_OEM_4 + KEY_OEM_3;
        }
        else if (m_focusItem == KEY_POINT) {
            m_focusItem = KEY_3;
        }
        else {
            m_focusItem = KEY_CANCEL;
        }
        m_btn[m_focusItem]->setFocus();
        break;
    case Qt::Key_Down:
        if (m_focusItem <= KEY_BACKSPACE) {
            m_focusItem = m_focusItem - KEY_OEM_1 + KEY_OEM_2;
        }
        else if (m_focusItem == KEY_OEM_2) {
            m_focusItem = KEY_OEM_3;
        }
        else if (m_focusItem <= KEY_MINUS) {
            m_focusItem = KEY_SPACE;
        }
        else if (m_focusItem <= KEY_6) {
            m_focusItem = m_focusItem - KEY_4 + KEY_1;
        }
        else if (m_focusItem == KEY_CANCEL) {
            m_focusItem = KEY_ENTER;
        }
        else if (m_focusItem <= KEY_SPACE) {
            m_focusItem = m_focusItem - KEY_OEM_3 + KEY_OEM_4;
        }
        else if (m_focusItem <= KEY_2) {
            m_focusItem = KEY_0;
        }
        else if (m_focusItem == KEY_3) {
            m_focusItem = KEY_POINT;
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
