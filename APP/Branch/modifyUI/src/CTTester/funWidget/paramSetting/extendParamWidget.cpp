#include "extendParamWidget.h"

#include <QGroupBox>
#include <QLabel>
#include <QListView>
#include <QBoxLayout>
#include <QLineEdit>
#include <QApplication>

#include "src/CTTester/overrideControl/mqcombobox.h"
#include "src/CTTester/overrideControl/mListWidget.h"
#include "src/CTTester/overrideControl/mDoubleValidator.h"

#include "src/CTTester/dataManager/dataManager.h"
#include "src/CTTester/dataManager/settings/ctBaseParamSetttings.h"
#include "src/CTTester/dataManager/settings/ptBaseParamSettings.h"
#include "src/CTTester/dataManager/settings/ctExtenParamSettings.h"

#define     CONTROL_MIN_HEIGHT      25
#define     MIN_DOUBLE_VALUE        0.00
#define     MAX_DOUBLE_VALUE        10000.00

CExtendParamWidget::CExtendParamWidget(QWidget *parent) : QGroupBox(parent)
{
    m_dataMgr = CDataManager::getInstance();

    this->setTitle(tr("扩展参数"));

    initWidget();
    localConnect();
    addEventFilter();
    onCurrentType();
}

void CExtendParamWidget::inductorChang(int type)
{
    if(type)
        onVoltageType();
    else
        onCurrentType();
}

void CExtendParamWidget::onChangeChecked(bool status)
{
    if(INDUCTOR_PT == m_dataMgr->getInductorType())
    {
        if(!status)
        {
            setAllControlHide();
        }
        else
        {
            onChangeCheckedShow();
        }
    }
}

void CExtendParamWidget::onLevelChanged(int index)
{
    if(INDUCTOR_CT == m_dataMgr->getInductorType())
    {
        switch (index) {
        case CTPT_P:
            onLevelPShow();                                                    // p级 显示
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
        case CTPT_TPX:
            onLevelTpxShow();                                                  //tpx级显示
            break;
        case CTPT_TPY:
            onLevelTpyShow();                                                  //tpy级  显示
            break;
        case CTPT_TPZ:
            onLevelTpzShow();                                                  //tpz级显示
            break;
        case CTPT_TPS:
            onLevelTpsShow();                                                  //tps级显示
            break;
        default:
            break;
        }
    }
}

void CExtendParamWidget::focusInEvent(QFocusEvent *event)
{
    focusNextChild();

    return QWidget::focusInEvent(event);
}

bool CExtendParamWidget::eventFilter(QObject *obj, QEvent *eve)
{
    if(m_pDutyCycle == obj ||m_pAlf == obj ||
       m_pKssc == obj || m_pKtd == obj || m_pTp == obj ||
       m_pTs == obj || m_pT1 == obj || m_pTal1 == obj ||
       m_pTfr == obj || m_pT2 == obj || m_pTal2 == obj ||
       m_pFS == obj || m_pExtendCurrent == obj || m_pKx == obj ||
       m_pEk == obj || m_pIe == obj ||
       m_pUal == obj || m_pIal == obj)
    {
        if(eve->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(eve);
            switch (keyEvent->key())
            {
            case Qt::Key_Left:
                this->parentWidget()->eventFilter(this,eve);
                return true;
            case Qt::Key_Right:
                this->parentWidget()->eventFilter(this,eve);
                return true;
            case Qt::Key_Up:
                if(m_pAlf == obj || m_pFS == obj || m_pKx == obj || m_pKssc == obj)
                    this->parentWidget()->eventFilter(this,eve);
                else
                    focusPreviousChild();
                return true;
            case Qt::Key_Down:
                focusNextChild();
                return true;
            case Qt::Key_Escape:
            {
                QKeyEvent *e = new QKeyEvent(keyEvent->type(),keyEvent->key(),Qt::NoModifier);
                QApplication::postEvent(this->parent(), e);
                return false;
            }
            default:
                break;
            }
        }
    }

    return false;
}

void CExtendParamWidget::onDutyCycChanged(int index)
{
    switch(index)
    {
    case 0:
        m_pTfr->hide();                                 //tfr
        m_pTfrL->hide();
        m_pT2->hide();                                  //t2
        m_pT2L->hide();
        m_pTal2->hide();                                //tal2
        m_pTal2L->hide();
        break;
    case 1:
        m_pTfr->show();                                 //tfr
        m_pTfrL->show();
        m_pT2->show();                                  //t2
        m_pT2L->show();
        m_pTal2->show();                                //tal2
        m_pTal2L->show();
        break;
    default:
        break;
    }

    CCTBaseParamSettings * ctBaseParam= m_dataMgr->ctBaseParamSettings();
    if(CTPT_TPX == ctBaseParam->testLevel()){
        CCTExtendParamSettings_TPX* tpxLevelParam = m_dataMgr->ctExtendParamTpxSettings();
        tpxLevelParam->setCoc(index);
    }
    else if(CTPT_TPY == ctBaseParam->testLevel()){
        CCTExtendParamSettings_TPY* tpyLevelParam = m_dataMgr->ctExtendParamTpySettings();
        tpyLevelParam->setCoc(index);
    }
}

void CExtendParamWidget::onEditingFinished(const QString &/*text*/)
{
    if(INDUCTOR_PT == m_dataMgr->getInductorType())
        return;

    CCTBaseParamSettings* baseParamSettings = m_dataMgr->ctBaseParamSettings();
    switch (baseParamSettings->testLevel())
    {
    case CTPT_P:
        setPLevelData();                                                    // p级
        break;
    case CTPT_MEASURE:
        setMeasureLevelData();                                              //计量级
        break;
    case CTPT_PR:
        setPrLevelData();                                                   //pr级
        break;
    case CTPT_PX:
        setPxLevelData();                                                   //px级
        break;
    case CTPT_TPX:
        setTpxLevelData();                                                  //tpx级
        break;
    case CTPT_TPY:
        setTpyLevelData();                                                  //tpy级
        break;
    case CTPT_TPZ:
        setTpzLevelData();                                                  //tpz级
        break;
    case CTPT_TPS:
        setTpsLevelData();                                                  //tps级
        break;
    default:
        break;
    }
}

void CExtendParamWidget::initWidget()
{
    m_pDutyCycle = new MQComboBox;                                                   // 工作循环
    MListWidget *dutyListView = new MListWidget();
    m_pDutyCycle->setModel(dutyListView->model());
    m_pDutyCycle->setView(dutyListView);
    QStringList dutyCycleList;
    dutyCycleList << tr("C-O") << tr("C-O-C-O");
    m_pDutyCycle->addItems(dutyCycleList);

    m_pAlf = new QLineEdit("10.0");                                 // 额定准确限值系数
    m_pKssc = new QLineEdit("10.0");                                // 额定对称短路电流系数
    m_pKtd = new QLineEdit("10.0");                                 // 暂态面积系数
    m_pTp = new QLineEdit("0.10");                                  // 一次时间常数
    m_pTs = new QLineEdit("3.00");                                  // 二次时间常数
    m_pT1 = new QLineEdit("0.10");                                  // t1
    m_pTal1 = new QLineEdit("0.04");                                // tal1
    m_pTfr = new QLineEdit("0.50");                                 // tfr
    m_pT2 = new QLineEdit("0.10");                                  // t2
    m_pTal2 = new QLineEdit("0.04");                                // tal2
    m_pFS = new QLineEdit("10.0");                                  // 额定仪表保安系数
    m_pExtendCurrent = new QLineEdit("120.0");                      // 扩展电流标定
    m_pKx = new QLineEdit("10.0");                                  // 额定计算系数Kx
    m_pEk = new QLineEdit("1.00");                                  // 额定拐点电势
    m_pIe = new QLineEdit("0.1");                                   // Ek对应le
    m_pUal = new QLineEdit("1.00");                                 // 额定ual
    m_pIal = new QLineEdit("0.1");                                  // Ual对应最大lal

    m_pAlfL = new QLabel(tr("Alf:"));                               // 额定准确限值系数
    m_pKsscL = new QLabel(tr("Kssc:"));                             // 额定对称短路电流系数
    m_pKtdL = new QLabel(tr("Ktd:"));                               // 暂态面积系数
    m_pTpL = new QLabel(tr("Tp(s):"));                              // 一次时间常数
    m_pTsL = new QLabel(tr("Ts(s):"));                              // 二次时间常数
    m_pDutyCycleL = new QLabel(tr("工作循环:"));                     // 工作循环
    m_pT1L = new QLabel(tr("t1(s):"));                              // t1
    m_pTal1L = new QLabel(tr("tal1(s):"));                          // tal1
    m_pTfrL = new QLabel(tr("tfr(s):"));                            // tfr
    m_pT2L = new QLabel(tr("t2(s):"));                              // t2
    m_pTal2L = new QLabel(tr("tal2(s):"));                          // tal2
    m_pFSL = new QLabel(tr("FS:"));                                 // 额定仪表保安系数
    m_pExtendCurrentL = new QLabel(tr("扩展电流标定(%):"));           // 扩展电流标定
    m_pKxL = new QLabel(tr("Kx:"));                                 // 额定计算系数Kx
    m_pEkL = new QLabel(tr("Ek(V):"));                              // 额定拐点电势
    m_pIeL = new QLabel(tr("Ie(A):"));                              // Ek对应Ie
    m_pUalL = new QLabel(tr("Eal(V):"));                            // 额定Ual
    m_pIalL = new QLabel(tr("Ial(A):"));                            // Ual对应最大Ial

    setGroupLayout();
    setControlMinHeight();
    setLineEditsReg();
}

void CExtendParamWidget::onCurrentType()
{
    if(INDUCTOR_CT == CDataManager::getInstance()->getInductorType()){
        CCTBaseParamSettings * ctBaseParam= m_dataMgr->ctBaseParamSettings();
        onLevelChanged(ctBaseParam->testLevel());
    }
    else if(INDUCTOR_PT == CDataManager::getInstance()->getInductorType()){

    }
}

void CExtendParamWidget::onVoltageType()
{
    setAllControlHide();
}

void CExtendParamWidget::setControlMinHeight()
{
    m_pDutyCycle->setMinimumHeight(CONTROL_MIN_HEIGHT);                                      // 工作循环

    m_pAlf->setMinimumHeight(CONTROL_MIN_HEIGHT);                                           // 额定准确限值系数
    m_pKssc->setMinimumHeight(CONTROL_MIN_HEIGHT);                                          // 额定对称短路电流系数
    m_pKtd->setMinimumHeight(CONTROL_MIN_HEIGHT);                                           // 暂态面积系数
    m_pTp->setMinimumHeight(CONTROL_MIN_HEIGHT);                                            // 一次时间常数
    m_pTs->setMinimumHeight(CONTROL_MIN_HEIGHT);                                            // 二次时间常数
    m_pT1->setMinimumHeight(CONTROL_MIN_HEIGHT);                                            // t1
    m_pTal1->setMinimumHeight(CONTROL_MIN_HEIGHT);                                          // tal1
    m_pTfr->setMinimumHeight(CONTROL_MIN_HEIGHT);                                           // tfr
    m_pT2->setMinimumHeight(CONTROL_MIN_HEIGHT);                                            // t2
    m_pTal2->setMinimumHeight(CONTROL_MIN_HEIGHT);                                          // tal2
    m_pFS->setMinimumHeight(CONTROL_MIN_HEIGHT);                                            // 额定仪表保安系数
    m_pExtendCurrent->setMinimumHeight(CONTROL_MIN_HEIGHT);                                 // 扩展电流标定
    m_pKx->setMinimumHeight(CONTROL_MIN_HEIGHT);                                            // 额定计算系数Kx
    m_pEk->setMinimumHeight(CONTROL_MIN_HEIGHT);                                            // 额定拐点电势
    m_pIe->setMinimumHeight(CONTROL_MIN_HEIGHT);                                            // Ek对应le
    m_pUal->setMinimumHeight(CONTROL_MIN_HEIGHT);                                           // 额定ual
    m_pIal->setMinimumHeight(CONTROL_MIN_HEIGHT);                                           // Ual对应最大lal

    m_pAlfL->setMinimumHeight(CONTROL_MIN_HEIGHT);                                          // 额定准确限值系数
    m_pKsscL->setMinimumHeight(CONTROL_MIN_HEIGHT);                                         // 额定对称短路电流系数
    m_pKtdL->setMinimumHeight(CONTROL_MIN_HEIGHT);                                          // 暂态面积系数
    m_pTpL->setMinimumHeight(CONTROL_MIN_HEIGHT);                                           // 一次时间常数
    m_pTsL->setMinimumHeight(CONTROL_MIN_HEIGHT);                                           // 二次时间常数
    m_pDutyCycleL->setMinimumHeight(CONTROL_MIN_HEIGHT);                                    // 工作循环
    m_pT1L->setMinimumHeight(CONTROL_MIN_HEIGHT);                                           // t1
    m_pTal1L->setMinimumHeight(CONTROL_MIN_HEIGHT);                                         // tal1
    m_pTfrL->setMinimumHeight(CONTROL_MIN_HEIGHT);                                          // tfr
    m_pT2L->setMinimumHeight(CONTROL_MIN_HEIGHT);                                           // t2
    m_pTal2L->setMinimumHeight(CONTROL_MIN_HEIGHT);                                         // tal2
    m_pFSL->setMinimumHeight(CONTROL_MIN_HEIGHT);                                           // 额定仪表保安系数
    m_pExtendCurrentL->setMinimumHeight(CONTROL_MIN_HEIGHT);                                // 扩展电流标定
    m_pKxL->setMinimumHeight(CONTROL_MIN_HEIGHT);                                           // 额定计算系数Kx
    m_pEkL->setMinimumHeight(CONTROL_MIN_HEIGHT);                                           // 额定拐点电势
    m_pIeL->setMinimumHeight(CONTROL_MIN_HEIGHT);                                           // Ek对应le
    m_pUalL->setMinimumHeight(CONTROL_MIN_HEIGHT);                                          // 额定ual
    m_pIalL->setMinimumHeight(CONTROL_MIN_HEIGHT);                                          // Ual对应最大lal
}

void CExtendParamWidget::setGroupLayout()
{
    QHBoxLayout *groupLayout = new QHBoxLayout;
    groupLayout->setContentsMargins(15,15,15,0);

    QVBoxLayout *labelLayout = new QVBoxLayout;
    labelLayout->setSpacing(10);
    labelLayout->addWidget(m_pAlfL);                                // 额定准确限值系数
    labelLayout->addWidget(m_pKsscL);                               // 额定对称短路电流系数
    labelLayout->addWidget(m_pKtdL);                                // 暂态面积系数
    labelLayout->addWidget(m_pTpL);                                 // 一次时间常数
    labelLayout->addWidget(m_pTsL);                                 // 二次时间常数
    labelLayout->addWidget(m_pDutyCycleL);                          // 工作循环
    labelLayout->addWidget(m_pT1L);                                 // t1
    labelLayout->addWidget(m_pTal1L);                               // tal1
    labelLayout->addWidget(m_pTfrL);                                // tfr
    labelLayout->addWidget(m_pT2L);                                 // t2
    labelLayout->addWidget(m_pTal2L);                               // tal2
    labelLayout->addWidget(m_pFSL);                                 // 额定仪表保安系数
    labelLayout->addWidget(m_pExtendCurrentL);                      // 扩展电流标定
    labelLayout->addWidget(m_pKxL);                                 // 额定计算系数Kx
    labelLayout->addWidget(m_pEkL);                                 // 额定拐点电势
    labelLayout->addWidget(m_pIeL);                                 // Ek对应le
    labelLayout->addWidget(m_pUalL);                                // 额定ual
    labelLayout->addWidget(m_pIalL);                                // Ual对应最大lal
    labelLayout->addStretch();

    QVBoxLayout *controlLayout = new QVBoxLayout;
    controlLayout->setSpacing(10);
    controlLayout->addWidget(m_pAlf);                                   // 额定准确限值系数
    controlLayout->addWidget(m_pKssc);                                  // 额定对称短路电流系数
    controlLayout->addWidget(m_pKtd);                                   // 暂态面积系数
    controlLayout->addWidget(m_pTp);                                    // 一次时间常数
    controlLayout->addWidget(m_pTs);                                    // 二次时间常数
    controlLayout->addWidget(m_pDutyCycle);                             // 工作循环
    controlLayout->addWidget(m_pT1);                                    // t1
    controlLayout->addWidget(m_pTal1);                                  // tal1
    controlLayout->addWidget(m_pTfr);                                   // tfr
    controlLayout->addWidget(m_pT2);                                    // t2
    controlLayout->addWidget(m_pTal2);                                  // tal2
    controlLayout->addWidget(m_pFS);                                    // 额定仪表保安系数
    controlLayout->addWidget(m_pExtendCurrent);                         // 扩展电流标定
    controlLayout->addWidget(m_pKx);                                    // 额定计算系数Kx
    controlLayout->addWidget(m_pEk);                                    // 额定拐点电势
    controlLayout->addWidget(m_pIe);                                    // Ek对应le
    controlLayout->addWidget(m_pUal);                                   // 额定ual
    controlLayout->addWidget(m_pIal);                                   // Ual对应最大lal
    controlLayout->addStretch();

    groupLayout->addLayout(labelLayout);
    groupLayout->addStretch();
    groupLayout->addLayout(controlLayout);

    this->setLayout(groupLayout);
}

void CExtendParamWidget::localConnect()
{
    connect(m_pDutyCycle,SIGNAL(currentIndexChanged(int)),this,SLOT(onDutyCycChanged(int)));

    connect(m_pAlf,SIGNAL(textChanged(const QString &)),this,SLOT(onEditingFinished(const QString &)));
    connect(m_pKssc,SIGNAL(textChanged(const QString &)),this,SLOT(onEditingFinished(const QString &)));
    connect(m_pKtd,SIGNAL(textChanged(const QString &)),this,SLOT(onEditingFinished(const QString &)));
    connect(m_pTp,SIGNAL(textChanged(const QString &)),this,SLOT(onEditingFinished(const QString &)));
    connect(m_pTs,SIGNAL(textChanged(const QString &)),this,SLOT(onEditingFinished(const QString &)));
    connect(m_pT1,SIGNAL(textChanged(const QString &)),this,SLOT(onEditingFinished(const QString &)));
    connect(m_pTal1,SIGNAL(textChanged(const QString &)),this,SLOT(onEditingFinished(const QString &)));
    connect(m_pTfr,SIGNAL(textChanged(const QString &)),this,SLOT(onEditingFinished(const QString &)));
    connect(m_pT2,SIGNAL(textChanged(const QString &)),this,SLOT(onEditingFinished(const QString &)));
    connect(m_pTal2,SIGNAL(textChanged(const QString &)),this,SLOT(onEditingFinished(const QString &)));
    connect(m_pFS,SIGNAL(textChanged(const QString &)),this,SLOT(onEditingFinished(const QString &)));
    connect(m_pExtendCurrent,SIGNAL(textChanged(const QString &)),this,SLOT(onEditingFinished(const QString &)));
    connect(m_pKx,SIGNAL(textChanged(const QString &)),this,SLOT(onEditingFinished(const QString &)));
    connect(m_pEk,SIGNAL(textChanged(const QString &)),this,SLOT(onEditingFinished(const QString &)));
    connect(m_pIe,SIGNAL(textChanged(const QString &)),this,SLOT(onEditingFinished(const QString &)));
    connect(m_pUal,SIGNAL(textChanged(const QString &)),this,SLOT(onEditingFinished(const QString &)));
    connect(m_pIal,SIGNAL(textChanged(const QString &)),this,SLOT(onEditingFinished(const QString &)));
}

void CExtendParamWidget::setAllControlHide()
{
    m_pDutyCycle->hide();                                       // 工作循环

    m_pAlf->hide();                                             // 额定准确限值系数
    m_pKssc->hide();                                            // 额定对称短路电流系数
    m_pKtd->hide();                                             // 暂态面积系数
    m_pTp->hide();                                              // 一次时间常数
    m_pTs->hide();                                              // 二次时间常数
    m_pT1->hide();                                              // t1
    m_pTal1->hide();                                            // tal1
    m_pTfr->hide();                                             // tfr
    m_pT2->hide();                                              // t2
    m_pTal2->hide();                                            // tal2
    m_pFS->hide();                                              // 额定仪表保安系数
    m_pExtendCurrent->hide();                                   // 扩展电流标定
    m_pKx->hide();                                              // 额定计算系数Kx
    m_pEk->hide();                                              // 额定拐点电势
    m_pIe->hide();                                              // Ek对应le
    m_pUal->hide();                                             // 额定ual
    m_pIal->hide();                                             // Ual对应最大lal

    m_pAlfL->hide();                                            // 额定准确限值系数
    m_pKsscL->hide();                                           // 额定对称短路电流系数
    m_pKtdL->hide();                                            // 暂态面积系数
    m_pTpL->hide();                                             // 一次时间常数
    m_pTsL->hide();                                             // 二次时间常数
    m_pDutyCycleL->hide();                                      // 工作循环
    m_pT1L->hide();                                             // t1
    m_pTal1L->hide();                                           // tal1
    m_pTfrL->hide();                                            // tfr
    m_pT2L->hide();                                             // t2
    m_pTal2L->hide();                                           // tal2
    m_pFSL->hide();                                             // 额定仪表保安系数
    m_pExtendCurrentL->hide();                                  // 扩展电流标定
    m_pKxL->hide();                                             // 额定计算系数Kx
    m_pEkL->hide();                                             // 额定拐点电势
    m_pIeL->hide();                                             // Ek对应le
    m_pUalL->hide();                                            // 额定ual
    m_pIalL->hide();                                            // Ual对应最大lal
}

void CExtendParamWidget::addEventFilter()
{
    m_pAlf->installEventFilter(this);                                            // 额定准确限值系数
    m_pKssc->installEventFilter(this);                                           // 额定对称短路电流系数
    m_pKtd->installEventFilter(this);                                            // 暂态面积系数
    m_pTp->installEventFilter(this);                                             // 一次时间常数
    m_pTs->installEventFilter(this);                                             // 二次时间常数
    m_pDutyCycle->installEventFilter(this);                                      // 工作循环
    m_pT1->installEventFilter(this);                                             // t1
    m_pTal1->installEventFilter(this);                                           // tal1
    m_pTfr->installEventFilter(this);                                            // tfr
    m_pT2->installEventFilter(this);                                             // t2
    m_pTal2->installEventFilter(this);                                           // tal2
    m_pFS->installEventFilter(this);                                             // 额定仪表保安系数
    m_pExtendCurrent->installEventFilter(this);                                  // 扩展电流标定
    m_pKx->installEventFilter(this);                                             // 额定计算系数Kx
    m_pEk->installEventFilter(this);                                             // 额定拐点电势
    m_pIe->installEventFilter(this);                                             // Ek对应le
    m_pUal->installEventFilter(this);                                            // 额定ual
    m_pIal->installEventFilter(this);                                            // Ual对应最大lal
}

void CExtendParamWidget::onLevelPShow()
{
    setAllControlHide();
    // Data
    CCTExtendParamSettings_P *pLevelParam = m_dataMgr->ctExtendParamPSettings();
    m_pAlf->setText(QString::number(pLevelParam->alf(),'f',2));

    // UI
    m_pAlf->show();                                                     //额定准确限值系数
    m_pAlfL->show();
}

void CExtendParamWidget::onLevelTpyShow()
{
    setAllControlHide();
    // Data
    CCTExtendParamSettings_TPY *tpyLevelParam = m_dataMgr->ctExtendParamTpySettings();
    m_pKssc->setText(QString::number(tpyLevelParam->kssc(),'f',2));
    m_pKtd->setText(QString::number(tpyLevelParam->ktd(),'f',2));
    m_pTp->setText(QString::number(tpyLevelParam->Tp(),'f',2));
    m_pTs->setText(QString::number(tpyLevelParam->Ts(),'f',2));
    m_pDutyCycle->setCurrentIndex(tpyLevelParam->coc());
    m_pT1->setText(QString::number(tpyLevelParam->t1(),'f',2));
    m_pTal1->setText(QString::number(tpyLevelParam->tal1(),'f',2));
    m_pTfr->setText(QString::number(tpyLevelParam->tfr(),'f',2));
    m_pT2->setText(QString::number(tpyLevelParam->t2(),'f',2));
    m_pTal2->setText(QString::number(tpyLevelParam->tal2(),'f',2));

    // UI
    m_pKssc->show();                                            //额定对称短路电流系数
    m_pKsscL->show();
    m_pKtd->show();                                             //暂态面积系数
    m_pKtdL->show();
    m_pTp->show();                                              //一次时间常数
    m_pTpL->show();
    m_pTs->show();                                              //二次时间常数
    m_pTsL->show();
    m_pDutyCycle->show();                                       //工作循环
    m_pDutyCycleL->show();
    m_pT1->show();                                              //t1
    m_pT1L->show();
    m_pTal1->show();                                            //tal1
    m_pTal1L->show();

    if(1 == m_pDutyCycle->currentIndex())
    {
        onDutyCycChanged(1);
    }
}

void CExtendParamWidget::onLevelMeasureShow()
{
    setAllControlHide();
    // DATA
    CCTExtendParamSettings_Measure * measureLevelParam = m_dataMgr->ctExtendParamMeasureSettings();
    m_pFS->setText(QString::number(measureLevelParam->fs(),'f',2));                                 // 额定仪表保安系数
    m_pExtendCurrent->setText(QString::number(measureLevelParam->extendCurrent(),'f',2));           // 扩展电流标定

    // UI
    m_pFS->show();                                              //额定仪表保安系数
    m_pFSL->show();
    m_pExtendCurrent->show();                                   //扩展电流标定
    m_pExtendCurrentL->show();
}

void CExtendParamWidget::onLevelPrShow()
{
    setAllControlHide();
    // DATA
    CCTExtendParamSettings_PR * prLevelParam = m_dataMgr->ctExtendParamPrSettings();
    m_pAlf->setText(QString::number(prLevelParam->alf(),'f',2));            // 额定准确限值系数
    m_pTs->setText(QString::number(prLevelParam->Ts(),'f',2));              // 二次时间常数

    // UI
    m_pAlf->show();                                     //额定准确限值系数
    m_pAlfL->show();
    m_pTs->show();                                      //二次时间常数
    m_pTsL->show();
}

void CExtendParamWidget::onLevelPxShow()
{
    setAllControlHide();
    // DATA
    CCTExtendParamSettings_PX* pxLevelParam = m_dataMgr->ctExtendParamPxSettings();
    m_pKx->setText(QString::number(pxLevelParam->Kx(),'f',2));                      // 额定计算系数Kx
    m_pEk->setText(QString::number(pxLevelParam->Ek(),'f',2));                      // 额定拐点电势
    m_pIe->setText(QString::number(pxLevelParam->Ie(),'f',2));                      // Ek对应le

    // UI
    m_pKx->show();                                          //额定计算系数Kx
    m_pKxL->show();
    m_pEk->show();                                          //额定拐点电势
    m_pEkL->show();
    m_pIe->show();                                          //Ek对应le
    m_pIeL->show();
}

void CExtendParamWidget::onLevelTpsShow()
{
    setAllControlHide();
    // Data
    CCTExtendParamSettings_TPS* tpsLevelParam = m_dataMgr->ctExtendParamTpsSettings();
    m_pKssc->setText(QString::number(tpsLevelParam->kssc(),'f',2));                   // 额定对称短路电流系数
    m_pUal->setText(QString::number(tpsLevelParam->Ual(),'f',2));                     // 额定ual
    m_pIal->setText(QString::number(tpsLevelParam->Ial(),'f',2));                     // Ual对应最大lal

    // UI
    m_pKssc->show();                                       //额定对称短路电流系数
    m_pKsscL->show();
    m_pUal->show();                                        //额定ual
    m_pUalL->show();
    m_pIal->show();                                        //Ual对应最大lal
    m_pIalL->show();
}

void CExtendParamWidget::onLevelTpxShow()
{
    setAllControlHide();
    // DATA
    CCTExtendParamSettings_TPX *tpxLevelParam = m_dataMgr->ctExtendParamTpxSettings();
    m_pKssc->setText(QString::number(tpxLevelParam->kssc(),'f',2));
    m_pKtd->setText(QString::number(tpxLevelParam->ktd(),'f',2));
    m_pTp->setText(QString::number(tpxLevelParam->Tp(),'f',2));
    m_pTs->setText(QString::number(tpxLevelParam->Ts(),'f',2));
    m_pDutyCycle->setCurrentIndex(tpxLevelParam->coc());
    m_pT1->setText(QString::number(tpxLevelParam->t1(),'f',2));
    m_pTal1->setText(QString::number(tpxLevelParam->tal1(),'f',2));
    m_pTfr->setText(QString::number(tpxLevelParam->tfr(),'f',2));
    m_pT2->setText(QString::number(tpxLevelParam->t2(),'f',2));
    m_pTal2->setText(QString::number(tpxLevelParam->tal2(),'f',2));

    // UI
    m_pKssc->show();                                            //额定对称短路电流系数
    m_pKsscL->show();
    m_pKtd->show();                                             //暂态面积系数
    m_pKtdL->show();
    m_pTp->show();                                              //一次时间常数
    m_pTpL->show();
    m_pDutyCycle->show();                                       //工作循环
    m_pDutyCycleL->show();
    m_pT1->show();                                              //t1
    m_pT1L->show();
    m_pTal1->show();                                            //tal1
    m_pTal1L->show();

    if(1 == m_pDutyCycle->currentIndex())
    {
        onDutyCycChanged(1);
    }
}

void CExtendParamWidget::onLevelTpzShow()
{
    setAllControlHide();
    // Data
    CCTExtendParamSettings_TPZ* tpzLevelParam = m_dataMgr->ctExtendParamTpzSettings();
    m_pKssc->setText(QString::number(tpzLevelParam->kssc(),'f',2));         // 额定对称短路电流系数
    m_pKtd->setText(QString::number(tpzLevelParam->ktd(),'f',2));           // 暂态面积系数
    m_pTp->setText(QString::number(tpzLevelParam->Tp(),'f',2));             // 一次时间常数
    m_pTs->setText(QString::number(tpzLevelParam->Ts(),'f',2));             // 二次时间常数

    // UI
    m_pKssc->show();                                  //额定对称短路电流系数
    m_pKsscL->show();
    m_pKtd->show();                                   //暂态面积系数
    m_pKtdL->show();
    m_pTp->show();                                    //一次时间常数
    m_pTpL->show();
    m_pTs->show();                                    //二次时间常数
    m_pTsL->show();
}

void CExtendParamWidget::onChangeCheckedShow()
{

}

void CExtendParamWidget::setPLevelData()
{
    CCTExtendParamSettings_P *pLevelParam = m_dataMgr->ctExtendParamPSettings();
    pLevelParam->setAlf(m_pAlf->text().toDouble());          // 额定准确限值系数
}

void CExtendParamWidget::setTpyLevelData()
{
    CCTExtendParamSettings_TPY *tpyLevelParam = m_dataMgr->ctExtendParamTpySettings();
    tpyLevelParam->setKssc(m_pKssc->text().toDouble());
    tpyLevelParam->setKtd(m_pKtd->text().toDouble());
    tpyLevelParam->setTp(m_pTp->text().toDouble());
    tpyLevelParam->setTs(m_pTs->text().toDouble());
    tpyLevelParam->setT1(m_pT1->text().toDouble());
    tpyLevelParam->setTal1(m_pTal1->text().toDouble());
    tpyLevelParam->setTfr(m_pTfr->text().toDouble());
    tpyLevelParam->setT2(m_pT2->text().toDouble());
    tpyLevelParam->setTal2(m_pTal2->text().toDouble());
    tpyLevelParam->setCoc(m_pDutyCycle->currentIndex());
}

void CExtendParamWidget::setMeasureLevelData()
{
    CCTExtendParamSettings_Measure * measureLevelParam = m_dataMgr->ctExtendParamMeasureSettings();
    measureLevelParam->setFs(m_pFS->text().toDouble());                                 // 额定仪表保安系数
    measureLevelParam->setExtendCurrent(m_pExtendCurrent->text().toDouble());           // 扩展电流标定
}

void CExtendParamWidget::setPrLevelData()
{
    CCTExtendParamSettings_PR * prLevelParam = m_dataMgr->ctExtendParamPrSettings();
    prLevelParam->setAlf(m_pAlf->text().toDouble());                    // 额定准确限值系数
    prLevelParam->setTs(m_pTs->text().toDouble());                      // 二次时间常数
}

void CExtendParamWidget::setPxLevelData()
{
    CCTExtendParamSettings_PX* pxLevelParam = m_dataMgr->ctExtendParamPxSettings();
    pxLevelParam->setKx(m_pKx->text().toDouble());                              // 额定计算系数Kx
    pxLevelParam->setEk(m_pEk->text().toDouble());                              // 额定拐点电势
    pxLevelParam->setIe(m_pIe->text().toDouble());                              // Ek对应le
}

void CExtendParamWidget::setTpsLevelData()
{
    CCTExtendParamSettings_TPS* tpsLevelParam = m_dataMgr->ctExtendParamTpsSettings();
    tpsLevelParam->setKssc(m_pKssc->text().toDouble());                       // 额定对称短路电流系数
    tpsLevelParam->setKtd(m_pKtd->text().toDouble());                         // 面积系数Kx
    tpsLevelParam->setUal(m_pUal->text().toDouble());                         // 额定ual
    tpsLevelParam->setIal(m_pIal->text().toDouble());                         // Ual对应最大lal
}

void CExtendParamWidget::setTpxLevelData()
{
    CCTExtendParamSettings_TPX *tpxLevelParam = m_dataMgr->ctExtendParamTpxSettings();
    tpxLevelParam->setKssc(m_pKssc->text().toDouble());
    tpxLevelParam->setKtd(m_pKtd->text().toDouble());
    tpxLevelParam->setTp(m_pTp->text().toDouble());
    tpxLevelParam->setTs(m_pTs->text().toDouble());
    tpxLevelParam->setT1(m_pT1->text().toDouble());
    tpxLevelParam->setTal1(m_pTal1->text().toDouble());
    tpxLevelParam->setTfr(m_pTfr->text().toDouble());
    tpxLevelParam->setT2(m_pT2->text().toDouble());
    tpxLevelParam->setTal2(m_pTal2->text().toDouble());
    tpxLevelParam->setCoc(m_pDutyCycle->currentIndex());
}

void CExtendParamWidget::setTpzLevelData()
{
    CCTExtendParamSettings_TPZ *tpzLevelParam = m_dataMgr->ctExtendParamTpzSettings();
    tpzLevelParam->setKssc(m_pKssc->text().toDouble());       // 额定对称短路电流系数
    tpzLevelParam->setKtd(m_pKtd->text().toDouble());               // 暂态面积系数
    tpzLevelParam->setTp(m_pTp->text().toDouble());               // 一次时间常数
    tpzLevelParam->setTs(m_pTs->text().toDouble());               // 二次时间常数
}

void CExtendParamWidget::setPtExtendData()
{
    //   CPtExtendParamSettings * ptExtendParamSettings = m_pDataMgr->getPtExtendParamSettings();
}

void CExtendParamWidget::setLineEditsReg()
{
    MDoubleValidator *doubleReg = new MDoubleValidator(MIN_DOUBLE_VALUE,MAX_DOUBLE_VALUE,2,this);
    doubleReg->setNotation(QDoubleValidator::StandardNotation);                 // 设置非科学计数法
    m_pAlf->setValidator(doubleReg);                                 // 额定准确限值系数
    m_pKssc->setValidator(doubleReg);                                // 额定对称短路电流系数
    m_pKtd->setValidator(doubleReg);                                 // 暂态面积系数
    m_pTp->setValidator(doubleReg);                                  // 一次时间常数
    m_pTs->setValidator(doubleReg);                                  // 二次时间常数
    m_pT1->setValidator(doubleReg);                                  // t1
    m_pTal1->setValidator(doubleReg);                                // tal1
    m_pTfr->setValidator(doubleReg);                                 // tfr
    m_pT2->setValidator(doubleReg);                                  // t2
    m_pTal2->setValidator(doubleReg);                                // tal2
    m_pFS->setValidator(doubleReg);                                  // 额定仪表保安系数
    m_pExtendCurrent->setValidator(doubleReg);                       // 扩展电流标定
    m_pKx->setValidator(doubleReg);                                  // 额定计算系数Kx
    m_pEk->setValidator(doubleReg);                                  // 额定拐点电势
    m_pIe->setValidator(doubleReg);                                  // Ek对应le
    m_pUal->setValidator(doubleReg);                                 // 额定ual
    m_pIal->setValidator(doubleReg);                                 // Ual对应最大lal
}

