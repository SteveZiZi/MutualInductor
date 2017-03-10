#ifndef CRATIOWIDGET_H
#define CRATIOWIDGET_H

#include <QWidget>

#define COLUMN_COUNTS       7

class CDataManager;
class QGroupBox;
class QLineEdit;
class MQStandardItemModel;
class MQTableView;
class CRatioWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CRatioWidget(QWidget *parent = 0);

    void clearTestMark();

signals:

public slots:
    void updateWidget();

protected:
    void focusInEvent(QFocusEvent * event);
    bool eventFilter(QObject *obj, QEvent *eve);

private:
    void initWidget();
    void setMainLayout();
    void setLineEditEnable(bool enable);
    void addEventFilter();

    void updateRatioDiffView();
    void updatePhaseDiffView();

private:
    CDataManager                *m_dataMgr;

    QLineEdit                   *m_pRatio;                                  // 变比
    QLineEdit                   *m_pRatioDiff;                              // 比差
    QLineEdit                   *m_pPhaseDiff;                              // 相位
    QLineEdit                   *m_pTurnsratio;                             // 匝比
    QLineEdit                   *m_pTurnsratioErr;                          // 匝比误差
    QLineEdit                   *m_pPhasePolar;                             // 极性

    MQTableView                 *m_pRatioDiffView;                          // 变比表格
    MQTableView                 *m_pPhaseDiffView;                          // 相位表格

    MQStandardItemModel         *m_pRatioDiffModel;                         // 变比model
    MQStandardItemModel         *m_pPhaseDiffModel;                         // 相位model

    QGroupBox                   *m_pLeftGb;
    QGroupBox                   *m_pRightGb;
};

#endif // CRATIOWIDGET_H
