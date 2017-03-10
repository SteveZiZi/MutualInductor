#ifndef KEYBOARDDEMO_H
#define KEYBOARDDEMO_H

#include <QDialog>
#include "ui_keyboarddemo.h"

class keyboardDemo : public QDialog
{
    Q_OBJECT

public:
    keyboardDemo(QWidget *parent = 0);
    ~keyboardDemo();
    
private:
    Ui::keyboardDemoClass ui;
};

#endif // KEYBOARDDEMO_H
