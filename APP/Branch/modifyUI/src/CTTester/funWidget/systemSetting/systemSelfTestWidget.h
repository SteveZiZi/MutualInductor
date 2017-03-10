#ifndef CSYSTEMSELFTESTWIDGET_H
#define CSYSTEMSELFTESTWIDGET_H

#include <QWidget>

class QGroupBox;
class QLineEdit;
class MPushButton;
class CDataManager;

class CSystemSelfTestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CSystemSelfTestWidget(QWidget *parent = 0);

signals:
    void enableFunTab(bool status);

public slots:

protected:
    void focusInEvent(QFocusEvent * event);
    bool eventFilter(QObject *obj, QEvent *eve);

private slots:
    void onBtnClicked();

private:
    void initWidget();
    void setMainLayout();
    void setCtrlsMinSize();
    void localConnect();
    void addEventFilter();
    int popSwitchCtrlDlg(const QString& title, const QString& msg);

private:
    CDataManager                       *m_dataMgr;

    QLineEdit                          *m_pTestCurrent;                            // 测试电流
    QLineEdit                          *m_pTestVolgate;                            // 测试电压
    QLineEdit                          *m_pTestFrequeny;                           // 测试频率
    QLineEdit                          *m_pActuallCurrent;                         // 实际电流
    QLineEdit                          *m_pActuallVolgate;                         // 实际电压

    MPushButton                        *m_pStartTestBtn;                           // 开始测试
    MPushButton                        *m_pStopTestBtn;                            // 停止测试

    QGroupBox                          *m_pSysSelfTestGb;

    bool                                m_bMessageBoxPoped;
};

#endif // CSYSTEMSELFTESTWIDGET_H
