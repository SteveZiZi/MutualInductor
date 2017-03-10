#pragma once

#include "ui_numpad.h"
#include "mPushButton.h"

class QTimer;
class CNumPad : public QWidget
{
    Q_OBJECT
public:
    CNumPad(QWidget* parent = 0, Qt::WindowFlags f = 0);
    ~CNumPad();

    enum { KEY_NUM = 24 };

    void setInputLineEdit(QLineEdit *lineEdit) { m_inputLineEdit = lineEdit; }

    void resetWidget(void);

    QPushButton* focusedObj() const {return m_btn[m_focusItem];}

signals:
    void changeIm(int type);
    void commit(bool actived);

public slots:
    bool eventFilter(QObject *, QEvent *); 
    void onTimerOut();

private slots:
    void clickButton(int index);

private:
    bool dirKeyPress(int dirKey);
    
private:
    Ui::numPad ui;

    QLineEdit* m_inputLineEdit;
    MPushButton *m_btn[KEY_NUM];
    
    int m_focusItem;
    QTimer* m_pTimer;                               // 连续删除定时器
};
