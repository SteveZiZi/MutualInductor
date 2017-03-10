#ifndef CEXCITATIONWIDGET_H
#define CEXCITATIONWIDGET_H

#include <QWidget>
#include "src/CTTester/diagramCtrl/diagramCommon.h"
#include "src/CTTester/diagramCtrl/diagramCtrl.h"
#include "src/CTTester/diagramCtrl/Scale.h"
#include "src/CTTester/diagramCtrl/TrendCtrl/Trend.h"


#define DELC_SHOW_RESULT(name) \
    QLabel* m_p##name##L; \
    QLineEdit* m_p##name

class CDataManager;
class QLabel;
class QGroupBox;
class QLineEdit;
class MPushButton;
class QStackedWidget;
class MQStandardItemModel;
class MQTableView;
class CExcitationWidget : public QWidget
{
    enum
    {
        MAX_POINT_COUNT = 1000,
    };

    enum E_Data_Type
    {
        EXCITATION_TYPE,
        ERROR_5_TYPE,
        ERROR_10_TYPE
    };
    enum E_CHART_Type
    {
        CHART_TREND,
        CHART_TABLE
    };

    Q_OBJECT
public:
    explicit CExcitationWidget(QWidget *parent = 0);
    ~CExcitationWidget();

    void clearTestMark();

signals:

public slots:
    void updateWidget();

protected:
    void focusInEvent(QFocusEvent * event);
    bool eventFilter(QObject *obj, QEvent *eve);

private slots:
    void onBtnClicked();

private:
    void updateShow();

    void initWidget();
    void setCtrlsHide();
    void localConnect();
    void addEventFilter();

    void createTrend();
    void updateTrend();
    void clearTrend();

    void updateDataView();
    void clearDataView();

    void onLevelPShow();                                                    // p级 显示
    void onLevelTpyShow();                                                  // tpy级  显示
    void onLevelMeasureShow();                                              // 计量级 显示
    void onLevelPrShow();                                                   // pr级
    void onLevelPxShow();                                                   // px级
    void onLevelTpsShow();                                                  // tps级显示
    void onLevelTpxShow();                                                  // tpx级显示
    void onLevelTpzShow();                                                  // tpz级显示
    void onTPTypeShow();                                                    // TP测试结果

    void displayResult();
    void clearResult();

private:
    typedef MDIAGRAM::CTrend<double, double, 1, MAX_POINT_COUNT> DataTrend;

    CDataManager                *m_pDataMgr;                                        // 数据管理

    // 曲线和数据
    QStackedWidget              *m_pStackedWidget;
    MDIAGRAM::CDiagramCtrl      *m_pDiagram;                                        // 曲线图形界面
    MDIAGRAM::ScaleInfo         m_tScaleInfo;                                       // XY刻度尺参数信息
    MDIAGRAM::CScale            *m_pScale;                                          // XY刻度尺
    DataTrend                   *m_pTrend;                                          // 趋势曲线

    MQStandardItemModel         *m_pTableModel;
    MQTableView                 *m_pTableView;

    E_Data_Type                 m_eDataType;                                        // 当前显示的数据类型
    E_CHART_Type                m_eChartType;                                       // 当前显示图表的类型

    MPushButton                 *m_pItemDataTypeBtn;                                // 要现实的数据类型
    MPushButton                 *m_pExcitationBtn;                                  // 励磁数据
    MPushButton                 *m_p5ErrorInfoBtn;                                  // 5%误差数据
    MPushButton                 *m_p10ErrorIndoBtn;                                 // 10%误差数据

    DELC_SHOW_RESULT(KneeVoltage);          // 拐点电压(V)
    DELC_SHOW_RESULT(KneeCurrent);          // 拐点电流（A）
    DELC_SHOW_RESULT(LimitCoefficient);     // 准确限值系数ALF
    DELC_SHOW_RESULT(CompositeError);       // 复合误差εc（%）
    DELC_SHOW_RESULT(SafetyFactor);         // 仪表保安系数FS
    DELC_SHOW_RESULT(Eal);                  // 极限电动势Eal（V）
    DELC_SHOW_RESULT(Kr);                   // 剩磁系数Kr（%）
    DELC_SHOW_RESULT(SecondTime);           // 二次时间常数Ts（s)
    DELC_SHOW_RESULT(Lu);                   // 不饱和电感Lu（H）
    DELC_SHOW_RESULT(Kssc);                 // 短路倍数Kssc
    DELC_SHOW_RESULT(Ktd);                  // 暂态面积系数Ktd
    DELC_SHOW_RESULT(ErrorFactor);          // 峰瞬误差ε^（%）
    DELC_SHOW_RESULT(Ls);                   // 饱和电感Ls（H）
    DELC_SHOW_RESULT(Ek);                   // Ek
    DELC_SHOW_RESULT(Ekie);                 // Ek对应Ie
    DELC_SHOW_RESULT(Kx);                   // 计算系数
    DELC_SHOW_RESULT(Ual);                  // 额定
    DELC_SHOW_RESULT(Lal);                  // 实测

    QGroupBox                   *m_pLeftGb;                                         // 左侧结构
    QGroupBox                   *m_pRightGb;                                        // 右侧结构

};

#endif // CEXCITATIONWIDGET_H
