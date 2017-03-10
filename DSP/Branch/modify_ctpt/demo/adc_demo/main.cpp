#include "adc_demo.h"
#include <QApplication>
#include <QFile>
#include <QFont>

#include "application.h"

int main(int argc, char *argv[])
{
    Application a(argc, argv);
    // 设置字体
    QFont font("simsun", 12, QFont::Bold);
    a.setFont(font);

    settingsWidget w;
    w.show();
    w.Init();

//    QFile file(":/resource/qss/vkeyboard.qss");
//    if(file.open(QFile::ReadOnly))
//    {
//        a.setStyleSheet(file.readAll());
//        file.close();
//    }

    return a.exec();
}
