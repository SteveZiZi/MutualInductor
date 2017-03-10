#include "keyboarddemo.h"
#include <QApplication>
#include <QFile>

#include "application.h"

int main(int argc, char *argv[])
{
    Application a(argc, argv);
    keyboardDemo w;
    w.show();

    QFile file(":/resource/qss/vkeyboard.qss");
    if(file.open(QFile::ReadOnly))
    {
        a.setStyleSheet(file.readAll());
        file.close();
    }

    return a.exec();
}
