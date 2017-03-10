#ifndef CPARAMSETTINGWIDGET_H
#define CPARAMSETTINGWIDGET_H

#include <QWidget>
#include "basePageWidget.h"

class CMutualInductorTypeWidget;
class CTestItemWidget;
class CBaseParamSettingsWidget;
class CExtendParamWidget;
class CBottomStatusWidget;


class CParamSettingWidget : public CBasePageWidget
{
    Q_OBJECT
public:
    explicit CParamSettingWidget(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent *event);

    bool isManualStoped() const;
    int btnsCount() const;

signals:
    void enableFunTab(bool status);
    void startTest();
    void stopTest();
    void displayResult();
    void reloadParam(int type);

public slots:

protected:
    void focusInEvent(QFocusEvent * event);
    bool eventFilter(QObject *obj, QEvent *eve);
    void paintEvent(QPaintEvent*);


private slots:
    void onStartTest();
    void onStopTest();                              // 手动停止实验
    void stopTestAuto();                          // 实验自动结束
    void onDisplayResult();
    void onLoadParam();
    void onSaveParam();

private:
    void initMenu();
    void initWidget();
    void localConnection();
    void addEventFilter();
    void onStartTestBtnClicked();
    void onStopTestBtnClicked();
    int popSwitchCtrlDlg(const QString &title, const QString &msg);
    void setControlsEnable(bool flag);

private:

    CMutualInductorTypeWidget           *m_pMutualInductorTypeWidget;               // 互感器类型
    CTestItemWidget                     *m_pTestItemWidget;                         // 试验项目
    CBaseParamSettingsWidget            *m_pBaseParamSettingsWidget;                // 基本参数设置页
    CExtendParamWidget                  *m_pExtendParamWidget;                      // 扩展参数
    CBottomStatusWidget                 *m_pBottomStatusWidget;                     // 底部状态显示栏

    bool                                 m_bMessageBoxPoped;
    bool                                 m_bManualStoped;                           // 是否人为停止
};

#endif // CPARAMSETTINGWIDGET_H
