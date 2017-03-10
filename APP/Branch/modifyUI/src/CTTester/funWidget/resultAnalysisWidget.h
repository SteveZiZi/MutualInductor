#ifndef CRESULTANALYSISWIDGET_H
#define CRESULTANALYSISWIDGET_H

#include <QWidget>
#include "basePageWidget.h"

class CExcitationWidget;
class CResistanceLoadWidget;
class CRatioWidget;
class CSaveReportWidget;
class QStackedWidget;

class CResultAnalysisWidget : public CBasePageWidget
{
    Q_OBJECT
public:
    explicit CResultAnalysisWidget(QWidget *parent = 0);

signals:

public:
    void updateResult();
    void clearResult();
    void displayExcitationResult();

public slots:

protected:
    void focusInEvent(QFocusEvent * event);
    bool eventFilter(QObject *obj, QEvent *eve);

private slots:
    void actionTrigger();

private:
    void initWidget();
    void initMenu();
    void setMainLayout();
    void localConnect();
    void addEventFilter();

private:
    QStackedWidget                  *m_pStackedWidget;
    CExcitationWidget               *m_pExcitationWidget;                       // 励磁数据
    CResistanceLoadWidget           *m_pResistanceLoadWidget;                   // 电阻负荷
    CRatioWidget                    *m_pRatioWidget;                            // 变比数据
    CSaveReportWidget               *m_pSaveReportWidget;                       // 保存报告
};

#endif // CRESULTANALYSISWIDGET_H
