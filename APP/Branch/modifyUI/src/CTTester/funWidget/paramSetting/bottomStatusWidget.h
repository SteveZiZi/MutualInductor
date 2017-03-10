#ifndef CBOTTOMSTATUSWIDGET_H
#define CBOTTOMSTATUSWIDGET_H

#include <QWidget>

class QLabel;
class QTimer;
class CDataManager;
class CBottomStatusWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CBottomStatusWidget(QWidget *parent = 0);

signals:
    void stopTest();
    void displayResult();

public slots:
    void onStartTest();
    void onStopTest();

protected:
    void focusInEvent(QFocusEvent * event);

private slots:
    void onUpdateStatus();

private:
    void initWidget();
    void localConnect();
    int popSwitchCtrlDlg(const QString &title, const QString &msg);

private:
    CDataManager                *m_dataMgr;

    QLabel                      *m_pStatusLabel;
    QTimer                      *m_pUpdateStatusTimer;

    bool                        m_bStopTest;                    // 人工停止测试
};

#endif // CBOTTOMSTATUSWIDGET_H
