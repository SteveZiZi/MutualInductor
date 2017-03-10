#include "ratioWidget.h"

#include <QLabel>
#include <QLineEdit>
#include <QHeaderView>
#include <QGroupBox>
#include <QFormLayout>
#include <QBoxLayout>
#include <QKeyEvent>
#include <QDebug>

#include "src/CTTester/overrideControl/mqstandarditemmodel.h"
#include "src/CTTester/overrideControl/mqtableview.h"
#include "src/CTTester/overrideControl/nofocusItemdelegate.h"
#include "src/CTTester/dataManager/settings/ctBaseParamSetttings.h"
#include "src/CTTester/dataManager/settings/ctExtenParamSettings.h"
#include "src/CTTester/dataManager/settings/ptBaseParamSettings.h"
#include "src/CTTester/dataManager/dataManager.h"

#define MIN_LINEEDIT_HEIGHT     30

enum {
    ITEM_VA,
    ITEM_FACTOR,
    ITEM_PERCENT_1,
    ITEM_PERCENT_5,
    ITEM_PERCENT_20,
    ITEM_PERCENT_100,
    ITEM_PERCENT_120
};

#define CREATE_REPORT(report)   \
    report->clear();            \
    report->setHorizontalHeaderItem(ITEM_VA,         new QStandardItem("VA"));  \
    report->setHorizontalHeaderItem(ITEM_FACTOR,     new QStandardItem("COS")); \
    report->setHorizontalHeaderItem(ITEM_PERCENT_1,  new QStandardItem("1%"));  \
    report->setHorizontalHeaderItem(ITEM_PERCENT_5,  new QStandardItem("5%"));  \
    report->setHorizontalHeaderItem(ITEM_PERCENT_20, new QStandardItem("20%")); \
    report->setHorizontalHeaderItem(ITEM_PERCENT_100,new QStandardItem("100%"));\
    report->setHorizontalHeaderItem(ITEM_PERCENT_120,new QStandardItem("120%"))

#define UPDATE_REPORT_CONTENT(report, i, data) \
    report->setItem(i,ITEM_VA,          new QStandardItem(QString("%1").arg(data.VA,0,'f',2)));         \
    report->setItem(i,ITEM_FACTOR,      new QStandardItem(QString("%1").arg(data.factor,0,'f',2)));     \
    report->setItem(i,ITEM_PERCENT_1,   new QStandardItem(QString("%1").arg(data.percent1,0,'f',3)));   \
    report->setItem(i,ITEM_PERCENT_5,   new QStandardItem(QString("%1").arg(data.percent5,0,'f',3)));   \
    report->setItem(i,ITEM_PERCENT_20,  new QStandardItem(QString("%1").arg(data.percent20,0,'f',3)));  \
    report->setItem(i,ITEM_PERCENT_100, new QStandardItem(QString("%1").arg(data.percent100,0,'f',3))); \
    report->setItem(i,ITEM_PERCENT_120, new QStandardItem(QString("%1").arg(data.percent120,0,'f',3)))

CRatioWidget::CRatioWidget(QWidget *parent)
    : QWidget(parent)
    , m_dataMgr(CDataManager::getInstance())
{
    initWidget();
    addEventFilter();
}

void CRatioWidget::clearTestMark()
{
    m_pRatio->setText("");
    m_pRatioDiff->setText("");
    m_pTurnsratio->setText("");
    m_pTurnsratioErr->setText("");
    m_pPhaseDiff->setText("");
    m_pPhasePolar->setText("");

    CREATE_REPORT(m_pRatioDiffModel);
    CREATE_REPORT(m_pPhaseDiffModel);
}

void CRatioWidget::updateWidget()
{
    if(m_dataMgr->getResult())
    {
        const CTPT::ratio_result &ratio = m_dataMgr->ctPtResult().ratio;
        // 变比
        if(INDUCTOR_CT == m_dataMgr->getInductorType())
        {
            CCTBaseParamSettings* ctBaseParam = m_dataMgr->ctBaseParamSettings();

            double priVol = ctBaseParam->primaryCurrent();
            if (qAbs(ratio.ratio) > 0.0001) {
                double secVol = priVol/ratio.ratio;
                if (priVol > 99999.9)
                    m_pRatio->setText(QString("%1:%2").arg(priVol).arg(secVol,0,'f',2,QChar('0')));
                else if (priVol>9999.9)
                    m_pRatio->setText(QString("%1:%2").arg(priVol).arg(secVol,0,'f',3,QChar('0')));
                else
                    m_pRatio->setText(QString("%1:%2").arg(priVol).arg(secVol,0,'f',4,QChar('0')));
            }
            else {
                m_pRatio->setText("--");
            }
        }
        else if(INDUCTOR_PT == m_dataMgr->getInductorType())
        {
        }
        // 比值差
        m_pRatioDiff->setText(QString::number(ratio.ratio_error,'f',3));
        // 额定匝比
        m_pTurnsratio->setText(QString::number(ratio.turns_ratio,'f',3));
        // 匝比误差
        m_pTurnsratioErr->setText(QString::number(ratio.turns_ratio_error,'f',3));
        // 相位
        m_pPhaseDiff->setText(QString::number(ratio.phase_error,'f',3));
        // 极性
        if(ratio.phase_polar)
            m_pPhasePolar->setText(tr("+"));
        else
            m_pPhasePolar->setText(tr("-"));

        updateRatioDiffView();
        updatePhaseDiffView();
    }
    else
    {
        clearTestMark();
    }
}

void CRatioWidget::focusInEvent(QFocusEvent *event)
{
    focusNextChild();

    return QWidget::focusInEvent(event);
}

bool CRatioWidget::eventFilter(QObject *obj, QEvent *eve)
{
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(eve);
    if(m_pRatioDiffView == obj)
    {
        if(eve->type() == QEvent::KeyPress)
        {
            switch(keyEvent->key()) {
            case Qt::Key_Down:
                if(m_pRatioDiffView->currentIndex().row() + 1 == m_pRatioDiffModel->rowCount()) {
                    m_pRatioDiffView->clearFocus();
                    m_pPhaseDiffView->setFocus();
                    return true;
                }
                break;
            case Qt::Key_Left:
                m_pRatioDiffView->clearFocus();
                m_pPhaseDiffView->setFocus();
                return true;
            case Qt::Key_Right:
                m_pRatioDiffView->clearFocus();
                this->parentWidget()->parentWidget()->eventFilter(this,eve);
                return true;
            default:
                break;
            }
        }
    }
    else if(m_pPhaseDiffView == obj)
    {
        if(eve->type() == QEvent::KeyPress)
        {
            switch(keyEvent->key()) {
            case Qt::Key_Up:
                if(m_pPhaseDiffView->currentIndex().row() == 0) {
                    m_pPhaseDiffView->clearFocus();
                    m_pRatioDiffView->setFocus();
                    return true;
                }
                break;
            case Qt::Key_Left:
                m_pPhaseDiffView->clearFocus();
                m_pRatioDiffView->setFocus();
                return true;
            case Qt::Key_Right:
                m_pPhaseDiffView->clearFocus();
                this->parentWidget()->parentWidget()->eventFilter(this,eve);
                return true;
            default:
                break;
            }
        }
    }

    return false;
}

void CRatioWidget::initWidget()
{
    m_pRatio = new QLineEdit;
    m_pRatioDiff = new QLineEdit;                              // 比差
    m_pPhaseDiff = new QLineEdit;                              // 相位
    m_pTurnsratio = new QLineEdit;                             // 匝比
    m_pTurnsratioErr = new QLineEdit;                          // 匝比误差
    m_pPhasePolar = new QLineEdit;                             // 极性

    m_pRatioDiffView = new MQTableView;                          // 变比表格
    m_pRatioDiffView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pRatioDiffView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 设置每行内容自动适应宽度
    m_pRatioDiffView->verticalHeader()->setVisible(false);
    m_pRatioDiffView->setSelectionBehavior(QAbstractItemView::SelectRows);            // 设置按行选中
    m_pRatioDiffView->setSelectionMode (QAbstractItemView::SingleSelection);
    m_pRatioDiffView->setItemDelegate(new NoFocusItemDelegate);                        // 去掉选中时的虚框

    m_pPhaseDiffView = new MQTableView;                          // 相位表格
    m_pPhaseDiffView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pPhaseDiffView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 设置每行内容自动适应宽度
    m_pPhaseDiffView->verticalHeader()->setVisible(false);
    m_pPhaseDiffView->setSelectionBehavior(QAbstractItemView::SelectRows);            // 设置按行选中
    m_pPhaseDiffView->setSelectionMode (QAbstractItemView::SingleSelection);
    m_pPhaseDiffView->setItemDelegate(new NoFocusItemDelegate);                        // 去掉选中时的虚框

    m_pRatioDiffModel = new MQStandardItemModel(this);                // 变比model
    CREATE_REPORT(m_pRatioDiffModel);
    m_pRatioDiffView->setModel(m_pRatioDiffModel);

    m_pPhaseDiffModel = new MQStandardItemModel(this);                // 相位model
    CREATE_REPORT(m_pPhaseDiffModel);
    m_pPhaseDiffView->setModel(m_pPhaseDiffModel);

    m_pLeftGb = new QGroupBox(tr("额定条件下"));
    m_pLeftGb->setMaximumWidth(200);
    m_pRightGb = new QGroupBox;

    m_pRatio->setMinimumHeight(MIN_LINEEDIT_HEIGHT);                                  // 变比
    m_pRatioDiff->setMinimumHeight(MIN_LINEEDIT_HEIGHT);                              // 比差
    m_pPhaseDiff->setMinimumHeight(MIN_LINEEDIT_HEIGHT);                              // 相位
    m_pTurnsratio->setMinimumHeight(MIN_LINEEDIT_HEIGHT);                             // 匝比
    m_pTurnsratioErr->setMinimumHeight(MIN_LINEEDIT_HEIGHT);                          // 匝比误差
    m_pPhasePolar->setMinimumHeight(MIN_LINEEDIT_HEIGHT);                             // 极性

    setMainLayout();
    setLineEditEnable(false);
}

void CRatioWidget::setMainLayout()
{
    QFormLayout *leftGbLayout = new QFormLayout;
    leftGbLayout->setSpacing(15);
    leftGbLayout->setContentsMargins(5,25,5,5);

    leftGbLayout->addRow(new QLabel(tr("变  比")),m_pRatio);
    leftGbLayout->addRow(new QLabel(tr("比  差")),m_pRatioDiff);
    leftGbLayout->addRow(new QLabel(tr("角  差")),m_pPhaseDiff);
    leftGbLayout->addRow(new QLabel(tr("匝  比")),m_pTurnsratio);
    leftGbLayout->addRow(new QLabel(tr("匝比误差")),m_pTurnsratioErr);
    leftGbLayout->addRow(new QLabel(tr("极  性")),m_pPhasePolar);

    m_pLeftGb->setLayout(leftGbLayout);
    m_pLeftGb->setFocusPolicy(Qt::NoFocus);

    QVBoxLayout *rightGbLayout = new QVBoxLayout;
    rightGbLayout->addWidget(new QLabel(tr("比值表(%)(在额定电流%下)")));
    rightGbLayout->addWidget(m_pRatioDiffView);
    rightGbLayout->addWidget(new QLabel(tr("相位表(分)(在额定电流%下)")));
    rightGbLayout->addWidget(m_pPhaseDiffView);
    m_pRightGb->setLayout(rightGbLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_pLeftGb);
    mainLayout->addWidget(m_pRightGb);

    this->setLayout(mainLayout);
}

void CRatioWidget::setLineEditEnable(bool enable)
{
    m_pRatio->setEnabled(enable);                                  // 变比
    m_pRatioDiff->setEnabled(enable);                              // 比差
    m_pPhaseDiff->setEnabled(enable);                              // 相位
    m_pTurnsratio->setEnabled(enable);                             // 匝比
    m_pTurnsratioErr->setEnabled(enable);                          // 匝比误差
    m_pPhasePolar->setEnabled(enable);                             // 极性
}

void CRatioWidget::addEventFilter()
{
    m_pRatioDiffView->installEventFilter(this);
    m_pPhaseDiffView->installEventFilter(this);
}

/***********************************************************
 * @brief: 更新变比差数据
 * @param :input [void]
 * @Param :output [void]
 * @Date:  2016/6/21
 * *********************************************************/
void CRatioWidget::updateRatioDiffView()
{
    CREATE_REPORT(m_pRatioDiffModel);
    if(m_dataMgr->getResult()) {
        const std::vector<struct CTPT::ratio_error_data> &data = m_dataMgr->ctPtResult().ratio_error_data;
        if(data.size()) {
            int i = 0;
            Q_FOREACH(const struct CTPT::ratio_error_data& errData, data) {
                UPDATE_REPORT_CONTENT(m_pRatioDiffModel, i, errData);
                i++;
            }
        }
    }
}

void CRatioWidget::updatePhaseDiffView()
{
    CREATE_REPORT(m_pPhaseDiffModel);
    if(m_dataMgr->getResult()) {
        const std::vector<struct CTPT::ratio_error_data> &data = m_dataMgr->ctPtResult().phase_error_data;
        if(data.size()) {
            int i = 0;
            Q_FOREACH(const struct CTPT::ratio_error_data& errData, data) {
                UPDATE_REPORT_CONTENT(m_pPhaseDiffModel, i, errData);
                i++;
            }
        }
    }
}

