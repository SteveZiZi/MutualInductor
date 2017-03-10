/*  @file
 *  @brief QApplication类的重载，通过重新实现notify以实现背光的自动关闭
 *  @author MSZ
 *  @date 2016/03/10
 *  @version 0.1
 */
#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include "src/vKeyboard/vkeyboard.h"

class Application : public QApplication
{
    Q_OBJECT
public:
    Application(int & argc, char ** argv);

    bool notify(QObject *obj, QEvent *e);

private:
    void adjustDesktop();
    
private:
    vKeyboard m_keyboard;
};

#endif // APPLICATION_H
