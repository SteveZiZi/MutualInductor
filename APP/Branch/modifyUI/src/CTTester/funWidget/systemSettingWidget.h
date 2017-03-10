#ifndef CSYSTEMSETTINGWIDGET_H
#define CSYSTEMSETTINGWIDGET_H

#include <QWidget>
#include "basePageWidget.h"

class QStackedWidget;
class CSystemSelfTestWidget;
class CSystemDebugWidget;
class CSystemUpdateWidget;
class CSystemItemWidget;
class CSystemAboutWidget;

class CSystemSettingWidget : public CBasePageWidget
{
    Q_OBJECT
public:
    explicit CSystemSettingWidget(QWidget *parent = 0);

signals:
    void enableFunTab(bool status);

public slots:

protected:
    void focusInEvent(QFocusEvent * event);
    bool eventFilter(QObject *obj, QEvent *eve);

private slots:
    void actionTrigger();
    void onEnableFunTab(bool status);

private:
    void initWidget();
    void initMenu();
    void setMainLayout();
    void localConnect();
    void addEventFilter();

private:
    CSystemDebugWidget                  *m_pSystemDebugWidget;                      // 系统调试
    CSystemUpdateWidget                 *m_pSystemUpdateWidget;                     // 系统升级
    QStackedWidget                      *m_pStackedWidget;                          // 堆栈窗口
    CSystemAboutWidget                  *m_pSystemAboutWidget;                      // 系统信息
    CSystemSelfTestWidget               *m_pSystemSelfTestWidget;                   // 系统自测
};

#endif // CSYSTEMSETTINGWIDGET_H
