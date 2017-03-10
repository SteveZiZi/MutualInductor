#pragma once

#include "ui_characterPad.h"
#include "mPushButton.h"

class QTimer;
class CCharacterPad : public QWidget
{
    Q_OBJECT
public:
    CCharacterPad(QWidget* parent = 0, Qt::WindowFlags f = 0);
    ~CCharacterPad();

    enum {KEY_NUM = 33};

    void setInputLineEdit(QLineEdit *lineEdit) { m_inputLineEdit = lineEdit; }

    void resetWidget(void);

    QPushButton* focusedObj() const {return m_btn[m_focusItem];}

signals:
    void changeIm(int type);
    void commit(bool actived);

public slots:
    bool eventFilter(QObject *, QEvent *);

private slots:
    void clickButton(int index);
    void onTimerOut();
    
private:
    void changeToLower();
    void changeToUpper();
    bool dirKeyPress(int dirKey);

private:
    Ui::characterPad ui;

    QLineEdit* m_inputLineEdit;
    MPushButton *m_btn[KEY_NUM];

    bool m_bCaps;
    bool m_bShift;

    int m_focusItem;
    QTimer* m_pTimer;                               // 连续删除定时器
};
