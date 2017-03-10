#include <QIcon>
#include <QFile>
#include "ctTesterWidget.h"
#include "overrideControl/mqapplication.h"

int main(int argc, char *argv[])
{
    MQApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/resource/images/CTlogo.png"));
    // 设置字体
    QFont font("simsun", 12, QFont::Bold);
    a.setFont(font);


    CCtTesterWidget w;

    // 为应用导入样式
    QFile file(":/resource/qss/uicontrol.qss");
    if(file.open(QFile::ReadOnly))
    {
        a.setStyleSheet(file.readAll());
        file.close();
    }
    w.resize(800,480);
    w.move(0,0);
    w.show();

    return a.exec();
}
