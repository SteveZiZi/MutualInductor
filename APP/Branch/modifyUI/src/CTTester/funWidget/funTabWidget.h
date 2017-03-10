#ifndef CFUNTABWIDGET_H
#define CFUNTABWIDGET_H

#include <QList>
#include <QAction>

#include "overrideControl/mqtabwidget.h"

class CParamSettingWidget;
class CResultAnalysisWidget;
class CSystemSettingWidget;
class CHelpWidget;

class CFunTabWidget : public MQTabWidget
{
    Q_OBJECT
public:
    CFunTabWidget(QWidget *parent=0);
    ~CFunTabWidget();

protected:
    void focusInEvent(QFocusEvent * event);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void onCurrentChanged(int index);
    void onEnableFunTab(bool status);
    void onDisplayResult();

private:
    void initFunTabWidget();
    void loaclConnect();

private:
    CParamSettingWidget             *m_pParamSettingWidget;
    CResultAnalysisWidget           *m_pResultAnalysisWidget;
    CSystemSettingWidget            *m_pSystemSettingWidget;
    CHelpWidget                     *m_pHelpWidget;
};

#endif // CFUNTABWIDGET_H
