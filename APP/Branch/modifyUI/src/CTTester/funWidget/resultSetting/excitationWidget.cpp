#include "excitationWidget.h"

#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QBoxLayout>
#include <QDebug>
#include <QKeyEvent>
#include <math.h>

#include "src/CTTester/common.h"
#include "src/CTTester/overrideControl/mPushButton.h"
#include "src/CTTester/overrideControl/mqstandarditemmodel.h"
#include "src/CTTester/overrideControl/mqtableview.h"
#include "src/CTTester/overrideControl/nofocusItemdelegate.h"
#include "src/CTTester/dataManager/settings/ctBaseParamSetttings.h"
#include "src/CTTester/diagramCtrl/BackGround.h"
#include "src/CTTester/diagramCtrl/Foreground.h"
#include "src/CTTester/dataManager/dataManager.h"

#define STACKED_WIDGET_WIDTH        450
#define STACKED_WIDGET_HEIGHT       377

#define LEFT_GROUPBOX_WIDGTH        240

#define CTRL_MIN_HEIGHT             30
#define BTN_MIN_WIDTH               105

#define LINEEDIT_MAX_LENGTH         5


#define CREATE_BTN_MENU(btn, menuCaption, w, l) \
    btn = new MPushButton(menuCaption); \
    btn->setCheckable(true); \
    btn->setMinimumHeight(w);   \
    btn->setMinimumWidth(l)

#define CREATE_SHOW_RESULT(name, lableCaption, labelLayout, lineEditLayout) \
    m_p##name##L = new QLabel(lableCaption); \
    m_p##name##L->setMinimumHeight(CTRL_MIN_HEIGHT); \
    m_p##name = new QLineEdit(); \
    m_p##name->setMinimumHeight(CTRL_MIN_HEIGHT); \
    m_p##name->setMaxLength(LINEEDIT_MAX_LENGTH); \
    m_p##name->setEnabled(false); \
    labelLayout->addWidget(m_p##name##L); \
    lineEditLayout->addWidget(m_p##name)

#define SET_SHOW_RESULT_SHOW(name) \
    m_p##name##L->show(); \
    m_p##name->show()

#define SET_SHOW_RESULT_HIDE(name) \
    m_p##name##L->hide(); \
    m_p##name->hide()

#define SET_TABLE_HEADER_TITLE(table, title1, title2, title3) \
    table->setHorizontalHeaderItem(0,new QStandardItem(title1)); \
    table->setHorizontalHeaderItem(1,new QStandardItem(title2)); \
    table->setHorizontalHeaderItem(2,new QStandardItem(title3))


CExcitationWidget::CExcitationWidget(QWidget *parent)
    : QWidget(parent),
      m_pDataMgr(CDataManager::getInstance()),
      m_eDataType(EXCITATION_TYPE),
      m_eChartType(CHART_TREND)
{
    initWidget();
    localConnect();
    updateWidget();
    addEventFilter();
}

CExcitationWidget::~CExcitationWidget()
{

}

void CExcitationWidget::clearTestMark()
{
    updateShow();

    clearResult();
    clearTrend();
    clearDataView();
}

void CExcitationWidget::updateWidget()
{
    updateShow();

    displayResult();
    updateTrend();
    updateDataView();
}

void CExcitationWidget::focusInEvent(QFocusEvent *event)
{
    m_pItemDataTypeBtn->setFocus();

    return QWidget::focusInEvent(event);
}

bool CExcitationWidget::eventFilter(QObject *obj, QEvent *eve)
{
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(eve);
    if(m_pItemDataTypeBtn == obj || m_pExcitationBtn == obj ||
       m_p5ErrorInfoBtn == obj || m_p10ErrorIndoBtn == obj)
    {
        if(eve->type() == QEvent::KeyPress)
        {
            switch (keyEvent->key()) {
            case Qt::Key_Left:
                focusPreviousChild();
                return true;
            case Qt::Key_Up:
                if(CHART_TABLE == m_pStackedWidget->currentIndex()) {
                    m_pTableView->setFocus();
                    return true;
                }
                else {
                    focusPreviousChild();
                    return true;
                }
            case Qt::Key_Right:
            case Qt::Key_Down:
                focusNextChild();
                return true;
            default:
                break;
            }
        }
    }

    if(m_pTableView == obj)
    {
        if(eve->type() == QEvent::KeyPress)
        {
            switch (keyEvent->key())  {
            case Qt::Key_Down:
                if(m_pTableView->currentIndex().row() + 1 == m_pTableModel->rowCount()) {
                    m_pTableView->clearFocus();
                    m_pItemDataTypeBtn->setFocus();
                    return true;
                }
                break;
            case Qt::Key_Left:
                m_pTableView->clearFocus();
                m_pItemDataTypeBtn->setFocus();
                return true;
            case Qt::Key_Right:
                m_pTableView->clearFocus();
                this->parentWidget()->parentWidget()->eventFilter(this,eve);
                return true;
            case Qt::Key_Escape:
                m_pTableView->clearFocus();
                m_pItemDataTypeBtn->setFocus();
                return true;
            default:
                break;
            }
        }
    }

    return false;
}

void CExcitationWidget::onBtnClicked()
{
    if(m_pItemDataTypeBtn == sender())  {
        if(CHART_TABLE == m_eChartType) {
            m_eChartType = CHART_TREND;
            m_pExcitationBtn->setText(tr("励磁曲线"));
            m_p5ErrorInfoBtn->setText(tr("5%误差曲线"));
            m_p10ErrorIndoBtn->setText(tr("10%误差曲线"));
            m_pItemDataTypeBtn->setText(tr("数据"));
        }
        else {
            m_eChartType = CHART_TABLE;
            m_pExcitationBtn->setText(tr("励磁数据"));
            m_p5ErrorInfoBtn->setText(tr("5%误差数据"));
            m_p10ErrorIndoBtn->setText(tr("10%误差数据"));
            m_pItemDataTypeBtn->setText(tr("曲线"));
        }
    }
    else if(m_pExcitationBtn == sender()) {
        m_eDataType = EXCITATION_TYPE;

        m_pExcitationBtn->setChecked(true);
        m_p5ErrorInfoBtn->setChecked(false);
        m_p10ErrorIndoBtn->setChecked(false);
    }
    else if(m_p5ErrorInfoBtn == sender()) {
        m_eDataType = ERROR_5_TYPE;
        m_pExcitationBtn->setChecked(false);
        m_p5ErrorInfoBtn->setChecked(true);
        m_p10ErrorIndoBtn->setChecked(false);
    }
    else if(m_p10ErrorIndoBtn == sender()) {
        m_eDataType = ERROR_10_TYPE;
        m_pExcitationBtn->setChecked(false);
        m_p5ErrorInfoBtn->setChecked(false);
        m_p10ErrorIndoBtn->setChecked(true);
    }

    m_pStackedWidget->setCurrentIndex(m_eChartType);
    if (m_eChartType == CHART_TABLE)
        updateDataView();
    else
        updateTrend();
}

void CExcitationWidget::updateShow()
{
    if(INDUCTOR_CT == m_pDataMgr->getInductorType())
    {
        CCTBaseParamSettings * ctBaseParam = m_pDataMgr->ctBaseParamSettings();
        switch (ctBaseParam->testLevel()) {
        case CTPT_P:
            onLevelPShow();                                                    // p级 显示
            break;
        case CTPT_TPY:
            onLevelTpyShow();                                                  //tpy级  显示
            break;
        case CTPT_MEASURE:
            onLevelMeasureShow();                                              //计量级 显示
            break;
        case CTPT_PR:
            onLevelPrShow();                                                   //pr级
            break;
        case CTPT_PX:
            onLevelPxShow();                                                   //px级
            break;
        case CTPT_TPS:
            onLevelTpsShow();                                                  //tps级显示
            break;
        case CTPT_TPX:
            onLevelTpxShow();                                                  //tpx级显示
            break;
        case CTPT_TPZ:
            onLevelTpzShow();                                                  //tpz级显示
            break;
        default:
            break;
        }

        if(!m_p5ErrorInfoBtn->isEnabled())
            m_p5ErrorInfoBtn->setEnabled(true);
        if(!m_p10ErrorIndoBtn->isEnabled())
            m_p10ErrorIndoBtn->setEnabled(true);
    }
    else if(INDUCTOR_PT == m_pDataMgr->getInductorType())
    {
        onTPTypeShow();
    }
}

void CExcitationWidget::initWidget()
{
    CREATE_BTN_MENU(m_pItemDataTypeBtn, tr("数据"), CTRL_MIN_HEIGHT, BTN_MIN_WIDTH);
    CREATE_BTN_MENU(m_pExcitationBtn,   tr("励磁曲线"), CTRL_MIN_HEIGHT, BTN_MIN_WIDTH);
    CREATE_BTN_MENU(m_p5ErrorInfoBtn,   tr("5%误差曲线"), CTRL_MIN_HEIGHT, BTN_MIN_WIDTH);
    CREATE_BTN_MENU(m_p10ErrorIndoBtn,  tr("10%误差曲线"), CTRL_MIN_HEIGHT, BTN_MIN_WIDTH);
    m_pItemDataTypeBtn->setFixedWidth(BTN_MIN_WIDTH / 2);

    m_pLeftGb = new QGroupBox(tr("试验数据"));                          // 左侧结构
    m_pLeftGb->setMaximumWidth(LEFT_GROUPBOX_WIDGTH);
    m_pLeftGb->setFocusPolicy(Qt::NoFocus);
    m_pRightGb = new QGroupBox;                                        // 右侧结构
    m_pRightGb->setFocusPolicy(Qt::NoFocus);

    m_pTableModel = new MQStandardItemModel();
    m_pTableModel->setHorizontalHeaderItem(0,new QStandardItem("Num"));
    m_pTableModel->setHorizontalHeaderItem(1,new QStandardItem("Urms(V)"));
    m_pTableModel->setHorizontalHeaderItem(2,new QStandardItem("Irms(V)"));

    m_pTableView = new MQTableView;
    m_pTableView->setFixedWidth(STACKED_WIDGET_WIDTH - 22);
    m_pTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 设置每行内容自动适应宽度
    m_pTableView->verticalHeader()->setVisible(false);
    m_pTableView->setSelectionBehavior(QAbstractItemView::SelectRows);            // 设置按行选中
    //m_pTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pTableView->setModel(m_pTableModel);
    m_pTableView->setItemDelegate(new NoFocusItemDelegate);                        // 去掉选中时的虚框

    m_pStackedWidget = new QStackedWidget(m_pRightGb);
    m_pStackedWidget->setFixedSize(STACKED_WIDGET_WIDTH,STACKED_WIDGET_HEIGHT);
    createTrend();
    m_pStackedWidget->addWidget(m_pDiagram);
    m_pStackedWidget->addWidget(m_pTableView);

    QVBoxLayout *leftGbLeftLayout = new QVBoxLayout;
    QVBoxLayout *leftGbRightLayout = new QVBoxLayout;
    leftGbLeftLayout->setSpacing(10);
    leftGbRightLayout->setSpacing(10);
    CREATE_SHOW_RESULT(KneeVoltage,      tr("拐点电压(V):"),     leftGbLeftLayout, leftGbRightLayout);
    CREATE_SHOW_RESULT(KneeCurrent,      tr("拐点电流(A):"),     leftGbLeftLayout, leftGbRightLayout);
    CREATE_SHOW_RESULT(LimitCoefficient, tr("准确限值系数ALF:"),  leftGbLeftLayout, leftGbRightLayout);
    CREATE_SHOW_RESULT(CompositeError,   tr("复合误差εc(%):"),   leftGbLeftLayout, leftGbRightLayout);
    CREATE_SHOW_RESULT(SafetyFactor,     tr("保安系数FS:"),      leftGbLeftLayout, leftGbRightLayout);
    CREATE_SHOW_RESULT(Eal,              tr("极限电动势Eal:"),    leftGbLeftLayout, leftGbRightLayout);
    CREATE_SHOW_RESULT(Kr,               tr("剩磁系数Kr(%):"),    leftGbLeftLayout, leftGbRightLayout);
    CREATE_SHOW_RESULT(SecondTime,       tr("二次时间常数Ts:"),   leftGbLeftLayout, leftGbRightLayout);
    CREATE_SHOW_RESULT(Lu,               tr("不饱和电感Lu(H):"),  leftGbLeftLayout, leftGbRightLayout);
    CREATE_SHOW_RESULT(Kssc,             tr("短路倍数Kssc:"),    leftGbLeftLayout, leftGbRightLayout);
    CREATE_SHOW_RESULT(Ktd,              tr("暂态面积系数Ktd:"),  leftGbLeftLayout, leftGbRightLayout);
    CREATE_SHOW_RESULT(ErrorFactor,      tr("峰瞬误差ε^(%):"),   leftGbLeftLayout, leftGbRightLayout);
    CREATE_SHOW_RESULT(Ls,               tr("饱和电感Ls(H):"),   leftGbLeftLayout, leftGbRightLayout);
    CREATE_SHOW_RESULT(Ek,               tr("Ek:"),            leftGbLeftLayout, leftGbRightLayout);
    CREATE_SHOW_RESULT(Ekie,             tr("Ek对应Ie:"),       leftGbLeftLayout, leftGbRightLayout);
    CREATE_SHOW_RESULT(Kx,               tr("计算系数Kx:"),      leftGbLeftLayout, leftGbRightLayout);
    CREATE_SHOW_RESULT(Ual,              tr("额定Ual:"),        leftGbLeftLayout, leftGbRightLayout);
    CREATE_SHOW_RESULT(Lal,              tr("实测Ial:"),        leftGbLeftLayout, leftGbRightLayout);
    leftGbLeftLayout->addStretch();
    leftGbRightLayout->addStretch();

    QHBoxLayout *leftGbLayout = new QHBoxLayout;
    leftGbLayout->addLayout(leftGbLeftLayout);
    leftGbLayout->addStretch();
    leftGbLayout->addLayout(leftGbRightLayout);
    leftGbLayout->setContentsMargins(5,15,5,0);
    m_pLeftGb->setLayout(leftGbLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_pItemDataTypeBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_pExcitationBtn);
    buttonLayout->addWidget(m_p5ErrorInfoBtn);
    buttonLayout->addWidget(m_p10ErrorIndoBtn);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(m_pRightGb);
    rightLayout->addLayout(buttonLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_pLeftGb);
    mainLayout->addLayout(rightLayout);
    this->setLayout(mainLayout);
}

void CExcitationWidget::setCtrlsHide()
{
    SET_SHOW_RESULT_HIDE(LimitCoefficient);
    SET_SHOW_RESULT_HIDE(CompositeError);
    SET_SHOW_RESULT_HIDE(SafetyFactor);
    SET_SHOW_RESULT_HIDE(Eal);
    SET_SHOW_RESULT_HIDE(Kr);
    SET_SHOW_RESULT_HIDE(SecondTime);
    SET_SHOW_RESULT_HIDE(Ktd);
    SET_SHOW_RESULT_HIDE(ErrorFactor);
    SET_SHOW_RESULT_HIDE(Ls);
    SET_SHOW_RESULT_HIDE(Ek);
    SET_SHOW_RESULT_HIDE(Ekie);
    SET_SHOW_RESULT_HIDE(Kx);
    SET_SHOW_RESULT_HIDE(Ual);
    SET_SHOW_RESULT_HIDE(Lal);
}

void CExcitationWidget::localConnect()
{
    connect(m_pItemDataTypeBtn,SIGNAL(clicked()),this,SLOT(onBtnClicked()));
    connect(m_pExcitationBtn,SIGNAL(clicked()),this,SLOT(onBtnClicked()));
    connect(m_p5ErrorInfoBtn,SIGNAL(clicked()),this,SLOT(onBtnClicked()));
    connect(m_p10ErrorIndoBtn,SIGNAL(clicked()),this,SLOT(onBtnClicked()));
}

void CExcitationWidget::addEventFilter()
{
    m_pItemDataTypeBtn->installEventFilter(this);
    m_pExcitationBtn->installEventFilter(this);
    m_p5ErrorInfoBtn->installEventFilter(this);
    m_p10ErrorIndoBtn->installEventFilter(this);
    m_pTableView->installEventFilter(this);
}

void CExcitationWidget::createTrend()
{
    m_pDiagram = new MDIAGRAM::CDiagramCtrl(NULL, NULL, m_pStackedWidget);
    m_pDiagram->setGeometry(m_pStackedWidget->rect());

    m_tScaleInfo.gridCountX = 100;
    m_tScaleInfo.intervalX = 20;
    m_tScaleInfo.divIntervalX = 4;

    m_tScaleInfo.gridCountY = 100;
    m_tScaleInfo.intervalY = 20;
    m_tScaleInfo.divIntervalY = 4;

    m_tScaleInfo.magin.setMargin(50, 40, 85, 40);
    m_tScaleInfo.rectClient = m_pDiagram->rect();
    m_tScaleInfo.axisDirY = MDIAGRAM::POSITIVE;
    m_tScaleInfo.axisDirX = MDIAGRAM::POSITIVE;

    MDIAGRAM::CBackGround* pBackGround = new MDIAGRAM::CBackGround(MDIAGRAM::BACKGROUND_ID);
    m_pScale = new MDIAGRAM::CScale(MDIAGRAM::SCALE_ID, pBackGround);
    m_pScale->SetShowTitle(true, true);
    m_pScale->SetTitle("Irms(A)", "Urms(V)");
    m_pScale->AttachScale(&m_tScaleInfo);
    m_pScale->setScaleValPos(40, -1);
    m_pScale->SetTitleColor(qRgb(255,255,255));
    m_pScale->SetScaleValColor(qRgb(255,255,255));

    MDIAGRAM::CForeground* pForeground = new MDIAGRAM::CForeground(MDIAGRAM::FOREGROUND_ID);
    m_pTrend = new DataTrend(MDIAGRAM::TREND_ID, pForeground);
    m_pTrend->AttachScale(&m_tScaleInfo);
    m_pTrend->SetColor(0, qRgb(255,255,255));

    m_pDiagram->SetComponent(m_pScale, m_pTrend);
    m_pDiagram->Init();
}

void CExcitationWidget::updateTrend()
{
    clearTrend();

    if(!m_pDataMgr->getResult())
        return ;

    const S_CTPT_Result &result = m_pDataMgr->ctPtResult();
    std::vector<CTPT::ui_wave_data> wave;
    int xLoop = 0;
    switch(m_eDataType)
    {
    case EXCITATION_TYPE:
        xLoop = result.exciting_data.size();
        wave = result.exciting_data;
        m_pTrend->SetColor(0,qRgb(0,255,255));
        break;
    case ERROR_5_TYPE:
        xLoop = result.error5_data.size();
        for(int i = 0; i < xLoop; i++)
            wave.push_back(CTPT::ui_wave_data(result.error5_data[i].Zload, result.error5_data[i].Kalf));
        m_pTrend->SetColor(0,qRgb(255,0,255));
        break;
    case ERROR_10_TYPE:
        xLoop = result.error10_data.size();
        for(int i = 0; i < xLoop; i++)
            wave.push_back(CTPT::ui_wave_data(result.error10_data[i].Zload, result.error10_data[i].Kalf));
        m_pTrend->SetColor(0,qRgb(255,255,0));
        break;
    default:
        break;
    }

    if (xLoop == 0) {
        return;
    }

    DataTrend::WaveData data;
    m_pTrend->RemoveAll();
    double maxCurrent = wave[0].cur;
    double maxVolgate = wave[0].vol;
    for(int i = 0 ; i < xLoop; i++)
    {
        data.xData = wave[i].cur;
        data.yData[0] = wave[i].vol;
        m_pTrend->AppendData(data);

        if (maxCurrent < data.xData)
            maxCurrent = data.xData;
        if (maxVolgate < data.yData[0])
            maxVolgate = data.yData[0];
    }

    if (maxCurrent < 1.0)
        m_tScaleInfo.fScaleX = 1.0/m_tScaleInfo.gridCountX;
    else
        m_tScaleInfo.fScaleX = ((int)(maxCurrent*2+0.5))/2.0/m_tScaleInfo.gridCountX;

    if (maxVolgate < 1.0)
        m_tScaleInfo.fScaleY = 1.0/m_tScaleInfo.gridCountY;
    else
        m_tScaleInfo.fScaleY = ((int)(maxVolgate*2+0.5))/2.0/m_tScaleInfo.gridCountY;

    m_pDiagram->DrawBkgnd();
    m_pDiagram->ResetForgnd();
    m_pDiagram->DrawForegound();
    m_pDiagram->repaint();
}

void CExcitationWidget::clearTrend()
{
    switch(m_eDataType)
    {
    case EXCITATION_TYPE:
        m_pScale->SetTitle("Irms(A)", "Urms(V)");
        break;
    case ERROR_5_TYPE:
        m_pScale->SetTitle("ZI(Ω)",tr("M(倍数)"));
        break;
    case ERROR_10_TYPE:
        m_pScale->SetTitle("ZI(Ω)",tr("M(倍数)"));
        break;
    default:
        break;
    }

    m_pDiagram->DrawBkgnd();
    m_pDiagram->ResetForgnd();
    m_pDiagram->repaint();
}

void CExcitationWidget::updateDataView()
{
    if(m_pDataMgr->getResult())
    {
        const S_CTPT_Result &result = m_pDataMgr->ctPtResult();
        std::vector<CTPT::ui_wave_data> wave;
        int xLoop = 0;

        m_pTableModel->clear();
        switch(m_eDataType)
        {
        case EXCITATION_TYPE:
            xLoop = result.exciting_data.size();
            wave = result.exciting_data;
            SET_TABLE_HEADER_TITLE(m_pTableModel, tr("Num"), tr("Urms(V)"), tr("Irms(V)"));
            break;
        case ERROR_5_TYPE:
            xLoop = result.error5_data.size();
            for(int i = 0; i < xLoop; i++)
                wave.push_back(CTPT::ui_wave_data(result.error5_data[i].Zload, result.error5_data[i].Kalf));
            SET_TABLE_HEADER_TITLE(m_pTableModel, tr("Num"), tr("M(倍数)"), tr("ZI(Ω)"));
            break;
        case ERROR_10_TYPE:
            xLoop = result.error10_data.size();
            for(int i = 0; i < xLoop; i++)
                wave.push_back(CTPT::ui_wave_data(result.error10_data[i].Zload, result.error10_data[i].Kalf));
            SET_TABLE_HEADER_TITLE(m_pTableModel, tr("Num"), tr("M(倍数)"), tr("ZI(Ω)"));
            break;
        default:
            break;
        }

        if(xLoop) {
            for(int i = 0;i < xLoop;i++) {
                m_pTableModel->setItem(i,0,new QStandardItem(QString::number(i + 1)));
                m_pTableModel->setItem(i,1,new QStandardItem(QString("%1").arg(wave[i].vol,0,'f',4)));
                m_pTableModel->setItem(i,2,new QStandardItem(QString("%1").arg(wave[i].cur,0,'f',4)));
            }
        }
    }
    // 清空数据
    else
        clearDataView();
}

void CExcitationWidget::clearDataView()
{
    m_pTableModel->clear();

    switch(m_eDataType)
    {
    case EXCITATION_TYPE:
        SET_TABLE_HEADER_TITLE(m_pTableModel, tr("Num"), tr("Urms(V)"), tr("Irms(V)"));
        break;
    case ERROR_5_TYPE:
        SET_TABLE_HEADER_TITLE(m_pTableModel, tr("Num"), tr("M(倍数)"), tr("ZI(Ω)"));
        break;
    case ERROR_10_TYPE:
        SET_TABLE_HEADER_TITLE(m_pTableModel, tr("Num"), tr("M(倍数)"), tr("ZI(Ω)"));
        break;
    default:
        break;
    }
}

void CExcitationWidget::onLevelPShow()
{
    setCtrlsHide();
    SET_SHOW_RESULT_SHOW(Eal);
    SET_SHOW_RESULT_SHOW(CompositeError);
    SET_SHOW_RESULT_SHOW(Lu);
    SET_SHOW_RESULT_SHOW(Kr);
    SET_SHOW_RESULT_SHOW(SecondTime);
    SET_SHOW_RESULT_SHOW(LimitCoefficient);
}

void CExcitationWidget::onLevelTpyShow()
{
    setCtrlsHide();
    SET_SHOW_RESULT_SHOW(Eal);
    SET_SHOW_RESULT_SHOW(Ktd);
    SET_SHOW_RESULT_SHOW(Lu);
    SET_SHOW_RESULT_SHOW(Kr);
    SET_SHOW_RESULT_SHOW(SecondTime);
    SET_SHOW_RESULT_SHOW(Kssc);
    SET_SHOW_RESULT_SHOW(ErrorFactor);
}

void CExcitationWidget::onLevelMeasureShow()
{
    setCtrlsHide();
    SET_SHOW_RESULT_SHOW(Eal);
    SET_SHOW_RESULT_SHOW(CompositeError);
    SET_SHOW_RESULT_SHOW(Lu);
    SET_SHOW_RESULT_SHOW(Kr);
    SET_SHOW_RESULT_SHOW(SecondTime);
    SET_SHOW_RESULT_SHOW(SafetyFactor);
}

void CExcitationWidget::onLevelPrShow()
{
    setCtrlsHide();
    SET_SHOW_RESULT_SHOW(Eal);
    SET_SHOW_RESULT_SHOW(CompositeError);
    SET_SHOW_RESULT_SHOW(Lu);
    SET_SHOW_RESULT_SHOW(Kr);
    SET_SHOW_RESULT_SHOW(SecondTime);
    SET_SHOW_RESULT_SHOW(LimitCoefficient);
}

void CExcitationWidget::onLevelPxShow()
{
    setCtrlsHide();
    SET_SHOW_RESULT_SHOW(Eal);
    SET_SHOW_RESULT_SHOW(Ek);
    SET_SHOW_RESULT_SHOW(Ekie);
    SET_SHOW_RESULT_SHOW(Lu);
    SET_SHOW_RESULT_SHOW(Kr);
    SET_SHOW_RESULT_SHOW(SecondTime);
    SET_SHOW_RESULT_SHOW(Kx);
}

void CExcitationWidget::onLevelTpsShow()
{
    setCtrlsHide();
    SET_SHOW_RESULT_SHOW(Ual);
    SET_SHOW_RESULT_SHOW(Lal);
    SET_SHOW_RESULT_SHOW(Lu);
    SET_SHOW_RESULT_SHOW(Kr);
    SET_SHOW_RESULT_SHOW(SecondTime);
    SET_SHOW_RESULT_SHOW(Kssc);
}

void CExcitationWidget::onLevelTpxShow()
{
    setCtrlsHide();
    SET_SHOW_RESULT_SHOW(Eal);
    SET_SHOW_RESULT_SHOW(Ktd);
    SET_SHOW_RESULT_SHOW(Lu);
    SET_SHOW_RESULT_SHOW(Kr);
    SET_SHOW_RESULT_SHOW(SecondTime);
    SET_SHOW_RESULT_SHOW(Kssc);
    SET_SHOW_RESULT_SHOW(ErrorFactor);
}

void CExcitationWidget::onLevelTpzShow()
{
    setCtrlsHide();
    SET_SHOW_RESULT_SHOW(Eal);
    SET_SHOW_RESULT_SHOW(Ktd);
    SET_SHOW_RESULT_SHOW(Lu);
    SET_SHOW_RESULT_SHOW(Kr);
    SET_SHOW_RESULT_SHOW(SecondTime);
    SET_SHOW_RESULT_SHOW(Kssc);
    SET_SHOW_RESULT_SHOW(ErrorFactor);
}

void CExcitationWidget::onTPTypeShow()
{
    setCtrlsHide();

    m_p5ErrorInfoBtn->setEnabled(false);
    m_p10ErrorIndoBtn->setEnabled(false);
}

void CExcitationWidget::displayResult()
{
    if(m_pDataMgr->getResult())
    {
        const S_CTPT_Result &result = m_pDataMgr->ctPtResult();
        const CTPT::excitation_result &exicting_data = result.excitation;

        m_pKneeVoltage->setText(QString::number(exicting_data.Vkn,'f',4));      //拐点电压
        m_pKneeCurrent->setText(QString::number(exicting_data.Ikn,'f',4));      //拐点电流
        m_pLu->setText(QString::number(exicting_data.Lu,'f',4));                //不饱和电感
        m_pSecondTime->setText(QString::number(exicting_data.Ts,'f',4));        //二次时间常数
        m_pKr->setText(QString::number(exicting_data.Kr,'f',4));                //剩磁系数

        switch(result.level) {
        case CTPT_MEASURE:
            //极限电动势Eal
            m_pEal->setText(QString::number(exicting_data.measure.Eal,'f',4));
            // 复合误差
            m_pCompositeError->setText(QString::number(exicting_data.measure.composite_error,'f',4));
            //保安系数
            m_pSafetyFactor->setText(QString::number(exicting_data.measure.fs,'f',4));
            break;
        case CTPT_P:
            //极限电动势Eal
            m_pEal->setText(QString::number(exicting_data.p.Eal,'f',4));
            // 复合误差
            m_pCompositeError->setText(QString::number(exicting_data.p.composite_error,'f',4));
            //准确限制系数
            m_pLimitCoefficient->setText(QString::number(exicting_data.p.alf,'f',4));
            break;
        case CTPT_PR:
            //极限电动势Eal
            m_pEal->setText(QString::number(exicting_data.pr.Eal,'f',4));
            // 复合误差
            m_pCompositeError->setText(QString::number(exicting_data.pr.composite_error,'f',4));
            //准确限制系数
            m_pLimitCoefficient->setText(QString::number(exicting_data.pr.alf,'f',4));
            break;
        case CTPT_PX:
            m_pKx->setText(QString::number(exicting_data.px.Kx,'f',4));
            m_pCompositeError->setText(QString::number(exicting_data.px.composite_error,'f',4));
            m_pEk->setText(QString::number(exicting_data.px.Ek,'f',4));
            m_pEkie->setText(QString::number(exicting_data.px.Ie,'f',4));
            break;
        case CTPT_TPX:
        case CTPT_TPY:
        case CTPT_TPZ:
            m_pEal->setText(QString::number(exicting_data.tpx.Eal,'f',4));
            m_pErrorFactor->setText(QString::number(exicting_data.tpx.peek_error,'f',4));   //峰瞬误差
            m_pKssc->setText(QString::number(exicting_data.tpx.kssc,'f',4));    //短路倍数
            m_pKtd->setText(QString::number(exicting_data.tpx.ktd,'f',4));      //暂态面积系数
            break;
        case CTPT_TPS:
            m_pKssc->setText(QString::number(exicting_data.tps.kssc,'f',4));
            m_pUal->setText(QString::number(exicting_data.tps.Ual,'f',4));
            m_pLal->setText(QString::number(exicting_data.tps.Ial,'f',4));
            break;
        }
    }
    else
    // 清空数据内容
    {
       clearResult();
    }
}

void CExcitationWidget::clearResult()
{
    //拐点电压
    m_pKneeVoltage->setText("");
    //拐点电流
    m_pKneeCurrent->setText("");
    //二次极限感应电势  double secondary_limiting_emf
    //准确限制系数
    m_pLimitCoefficient->setText("");
    // 复合误差
    m_pCompositeError->setText("");
    //保安系数
    m_pSafetyFactor->setText("");
    //极限电动势Eal
    m_pEal->setText("");
    //剩磁系数
    m_pKr->setText("");
    //二次时间常数
    m_pSecondTime->setText("");
    //不用饱和电感。
    m_pLu->setText("");
    //短路倍数
    m_pKssc->setText("");
    //暂态面积系数
    m_pKtd->setText("");
    //峰瞬误差
    m_pErrorFactor->setText("");
}
