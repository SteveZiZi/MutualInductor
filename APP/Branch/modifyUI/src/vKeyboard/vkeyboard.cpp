#include "vkeyboard.h"

#include <QPushButton>
#include <QGridLayout>
#include <QSignalMapper>
#include <QKeyEvent>
#include <QApplication>
#include <QRegExp>
#include <QLineEdit>

#include "characterPad.h"
#include "numPad.h"

vKeyboard::vKeyboard(QWidget *parent) 
    : QDialog(parent)
    , m_associatedLineEdit(NULL)
    , m_characterPad(NULL)
    , m_ImType(0)
    , m_bClearText(false)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);

    resize(800, 240);

    m_inputEdit = new QLineEdit(this);
    m_inputEdit->resize(800, 28);
    m_inputEdit->setFocusPolicy(Qt::NoFocus);
    m_inputEdit->move(0,6);
    m_inputEdit->installEventFilter(this);

    m_characterPad = new CCharacterPad(this);
    m_characterPad->setInputLineEdit(m_inputEdit);
    m_characterPad->move(0, 38);

    m_numPad = new CNumPad(this);
    m_numPad->setInputLineEdit(m_inputEdit);
    m_numPad->move(0, 38);

    connect(m_characterPad, SIGNAL(changeIm(int)), this, SLOT(changeIm(int)));
    connect(m_characterPad, SIGNAL(commit(bool)), this, SLOT(commit(bool)));
    connect(m_numPad, SIGNAL(changeIm(int)), this, SLOT(changeIm(int)));
    connect(m_numPad, SIGNAL(commit(bool)), this, SLOT(commit(bool)));

    if (m_ImType == 0) {
        m_characterPad->hide();
    }
    else {
        m_numPad->hide();
    }
}

vKeyboard::~vKeyboard()
{

}

bool vKeyboard::setRegExp(const QValidator *reg)
{
    m_inputEdit->setValidator(reg);
    if(reg){
        if(reg->inherits("QDoubleValidator") || reg->inherits("QIntValidator"))
            changeIm(0);
        else
            changeIm(1);
        return true;
    }

    return false;
}

void vKeyboard::setDisplayText(const QString &text)
{
    m_inputEdit->setText(text);
    m_inputEdit->selectAll();
    m_inputEdit->setFocus();
    m_bClearText = true;
}

void vKeyboard::changeIm(int type)
{
    if (m_ImType != type) {
        m_ImType = type;
        if (m_ImType == 0) {
            m_characterPad->hide();
            m_numPad->show();
            m_numPad->resetWidget();
        }
        else if (m_ImType == 1) {
            m_numPad->hide();
            m_characterPad->show();
            m_characterPad->resetWidget();
        }
    }
}

void vKeyboard::commit(bool actived)
{
    if (m_associatedLineEdit) {
        if (actived){
            if (m_associatedLineEdit->validator()){
                if(m_associatedLineEdit->validator()->inherits("QDoubleValidator"))
                    m_associatedLineEdit->setText(QString::number(m_inputEdit->text().toDouble(),'f',2));
                else if (m_associatedLineEdit->validator()->inherits("QIntValidator"))
                    m_associatedLineEdit->setText(QString::number(m_inputEdit->text().toInt()));
            }
            else
                m_associatedLineEdit->setText(m_inputEdit->text());
        }
        m_associatedLineEdit->deselect();
        m_associatedLineEdit = NULL;
    }
    m_inputEdit->setText("");
    hide();
}

bool vKeyboard::eventFilter(QObject *obj, QEvent *eve)
{
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(eve);
    if(m_inputEdit == obj)
    {
        if(QEvent::KeyPress == keyEvent->type())
        {
            switch (keyEvent->key())
            {
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_Down:
            case Qt::Key_Up:
            case Qt::Key_Left:
            case Qt::Key_Right:
            case Qt::Key_Escape:{
                if(m_characterPad->isVisible()){
                    m_characterPad->setFocus();
                    m_characterPad->eventFilter(m_characterPad->focusedObj(),eve);
                    return true;
                }
                else{
                    m_numPad->setFocus();
                    m_numPad->eventFilter(m_numPad->focusedObj(),eve);
                    return true;
                }
            }
            default:
                if (m_bClearText) {
                    m_inputEdit->setText("");
                    m_bClearText = false;
                }
                break;
            }
        }
    }

    return false;
}
