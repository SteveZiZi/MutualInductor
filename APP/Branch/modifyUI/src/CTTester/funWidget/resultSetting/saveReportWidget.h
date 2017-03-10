#ifndef CSAVEREPORTWIDGET_H
#define CSAVEREPORTWIDGET_H

#include <QWidget>

class QGroupBox;
class QLineEdit;
class MQCheckBox;
class MPushButton;
class CDataManager;

class CSaveReportWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CSaveReportWidget(QWidget *parent = 0);

signals:

public slots:

protected:
    void focusInEvent(QFocusEvent * event);
    bool eventFilter(QObject *obj, QEvent *eve);

private:
    void initWidget();
    void setMainLayout();
    void localConnect();
    void addEventFilter();

private:
    CDataManager                *m_dataMgr;

    //报告区域
    QGroupBox                   *m_pReportGb;
    QLineEdit                   *m_pReportName;
    QLineEdit                   *m_pTestTime;
    //保存项目
    MQCheckBox                  *m_pResistanceData;
    MQCheckBox                  *m_pExcitationData;
    MQCheckBox                  *m_pRatioData;
    MQCheckBox                  *m_pLoadData;
    QGroupBox                   *m_pSaveItemGb;
    MPushButton                 *m_pSaveBtn;
};

#endif // CSAVEREPORTWIDGET_H
