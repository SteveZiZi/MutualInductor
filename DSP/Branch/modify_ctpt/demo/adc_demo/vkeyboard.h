#ifndef VKEYBOARD_H
#define VKEYBOARD_H

#include <QDialog>

class QLineEdit;
class CCharacterPad;
class CNumPad;
class QValidator;

class vKeyboard : public QDialog
{
    Q_OBJECT
public:
    explicit vKeyboard(QWidget *parent = 0);
    ~vKeyboard();

    void setAssociatedLineEdit(QLineEdit * lineEdit) { m_associatedLineEdit = lineEdit; }
    QLineEdit* associatedLineEdit() { return m_associatedLineEdit; }

    bool setRegExp(const QValidator* reg);

    void setDisplayText(const QString &text);
  
public slots:
    void changeIm(int type);
    void commit(bool actived);

protected:
    bool eventFilter(QObject *obj, QEvent *eve);

private:
    QLineEdit *inputEdit;

    QLineEdit *m_associatedLineEdit;
    CCharacterPad* m_characterPad;
    CNumPad* m_numPad;

    int  m_ImType;
    bool m_bClearText;
};

#endif // VKEYBOARD_H
