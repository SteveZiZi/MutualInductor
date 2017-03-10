#include "baseParamSettingsWidget.h"

#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QDebug>
#include <QApplication>

#include "src/CTTester/common.h"
#include "src/CTTester/dataManager/device/paramdef.h"
#include "src/CTTester/dataManager/dataManager.h"
#include "src/CTTester/dataManager/settings/ctBaseParamSetttings.h"
#include "src/CTTester/dataManager/settings/ptBaseParamSettings.h"

#include "src/CTTester/overrideControl/mqcombobox.h"
#include "src/CTTester/overrideControl/mListWidget.h"
#include "src/CTTester/overrideControl/mDoubleValidator.h"


#define   NAME_GROUPBOX_L  0
#define   NAME_GROUPBOX_T  0
#define   NAME_GROUPBOX_W  BASE_PARAM_WIDGET_W
#define   NAME_GROUPBOX_H  60

#define   BASE_PARAM_L     0
#define   BASE_PARAM_T    (NAME_GROUPBOX_T+NAME_GROUPBOX_H+15)
#define   BASE_PARAM_W     BASE_PARAM_WIDGET_W
#define   BASE_PARAM_H     60

#define   RATIO_PARAM_L    0
#define   RATIO_PARAM_T   (BASE_PARAM_T+BASE_PARAM_H+15)
#define   RATIO_PARAM_W    BASE_PARAM_WIDGET_W
#define   RATIO_PARAM_H    100

#define   RES_PARAM_L      0
#define   RES_PARAM_T     (RATIO_PARAM_T+RATIO_PARAM_H+15)
#define   RES_PARAM_W      BASE_PARAM_WIDGET_W
#define   RES_PARAM_H      60


#define   ITEM_WIDTH       24
#define   ITEM_CONTENT_OFFSET  2   //编辑框纵坐标上移2个像素点

#define   MIN_DOUBLE_VALUE     0.000
#define   MAX_DOUBLE_VALUE     10000.00


CBaseParamSettingsWidget::CBaseParamSettingsWidget(QWidget *parent) : QWidget(parent)
{
    initWidget();

    connect(m_serialNumEdit,SIGNAL(textChanged(const QString &)),this,SLOT(finishEdit(const QString &)));
    connect(m_coilNumEdit,SIGNAL(textChanged(const QString &)),this,SLOT(finishEdit(const QString &)));

    connect(m_coilLevelCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(onLevelChanged(int)));
    connect(m_testCurEdit,SIGNAL(textChanged(const QString &)),this,SLOT(finishEdit(const QString &)));
    connect(m_primaryValEdit,SIGNAL(textChanged(const QString &)),this,SLOT(finishEdit(const QString &)));
    connect(m_secondValEdit,SIGNAL(textChanged(const QString &)),this,SLOT(finishEdit(const QString &)));

    connect(m_ratedLoadEdit,SIGNAL(textChanged(const QString &)),this,SLOT(finishEdit(const QString &)));
    connect(m_powerFactorEdit,SIGNAL(textChanged(const QString &)),this,SLOT(finishEdit(const QString &)));

    connect(m_testTempEdit,SIGNAL(textChanged(const QString &)),this,SLOT(finishEdit(const QString &)));
    connect(m_destTempEdit,SIGNAL(textChanged(const QString &)),this,SLOT(finishEdit(const QString &)));
}

void CBaseParamSettingsWidget::inductorChang(int type)
{
    if(type)
        onVoltageType();
    else
        onCurrentType();
}

void CBaseParamSettingsWidget::finishEdit(const QString &text)
{
    QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());

    if(INDUCTOR_CT == CDataManager::getInstance()->getInductorType()){
        CCTBaseParamSettings * ctBaseParam= CDataManager::getInstance()->ctBaseParamSettings();
        if (edit == m_serialNumEdit)
            ctBaseParam->setCtNum(text);
        else if (edit == m_coilNumEdit)
            ctBaseParam->setWindingNum(text);
        else if (edit == m_testCurEdit)
            ctBaseParam->setMaxTestCurrent(text.toDouble());
        else if (edit == m_primaryValEdit)
            ctBaseParam->setprimaryCurrent(text.toDouble());
        else if (edit == m_secondValEdit)
            ctBaseParam->setSecondCurrent(text.toDouble());
        else if (edit == m_ratedLoadEdit)
            ctBaseParam->setRatedLoad(text.toDouble());
        else if (edit == m_powerFactorEdit)
            ctBaseParam->setPowerFactor(text.toDouble());
        else if (edit == m_testTempEdit)
            ctBaseParam->setCurrentTemp(text.toDouble());
        else if (edit == m_destTempEdit)
            ctBaseParam->setDestTemp(text.toDouble());
    }
    else if(INDUCTOR_PT == CDataManager::getInstance()->getInductorType()){

    }
}

void CBaseParamSettingsWidget::onLevelChanged(int index)
{
    if (index == -1) {
        index = 0;
    }

    m_coilLevelCombo->setCurrentIndex(index);

    CDataManager* dataMgr = CDataManager::getInstance();
    if(INDUCTOR_CT == dataMgr->getInductorType())
    {
        CCTBaseParamSettings* baseParam = dataMgr->ctBaseParamSettings();
        baseParam->setTestLevel(static_cast<CTPT_CLASS_ENUM>(index));
    }
    else
    {
        CPTBaseParamSettings* baseParam = dataMgr->ptBaseParamSettings();
        baseParam->setTestLevel(static_cast<CTPT_CLASS_ENUM>(index));
    }

    emit inductorClassChanged(index);
}

void CBaseParamSettingsWidget::focusInEvent(QFocusEvent *event)
{
    focusNextChild();

    return QWidget::focusInEvent(event);
}

bool CBaseParamSettingsWidget::eventFilter(QObject *obj, QEvent *eve)
{
    if(eve->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(eve);
        switch (keyEvent->key())
        {
        case Qt::Key_Left:
            if(m_destTempEdit == obj)
                m_testTempEdit->setFocus();
            else if(m_powerFactorEdit == obj)
                m_ratedLoadEdit->setFocus();
            else if(m_secondValEdit == obj)
                m_primaryValEdit->setFocus();
            else if(m_testCurEdit == obj)
                m_coilLevelCombo->setFocus();
            else if(m_coilNumEdit == obj)
                m_serialNumEdit->setFocus();
            return true;
        case Qt::Key_Right:
            if(m_coilNumEdit == obj || m_testCurEdit == obj || m_secondValEdit == obj ||
               m_powerFactorEdit == obj || m_destTempEdit == obj)
                this->parentWidget()->eventFilter(this,eve);
            else if(m_serialNumEdit == obj)
                m_coilNumEdit->setFocus();
            else if(m_coilLevelCombo == obj)
                m_testCurEdit->setFocus();
            else if(m_primaryValEdit == obj)
                m_secondValEdit->setFocus();
            else if(m_ratedLoadEdit == obj)
                m_powerFactorEdit->setFocus();
            else if(m_testTempEdit == obj)
                m_destTempEdit->setFocus();
            return true;
        case Qt::Key_Up:
            if(m_testTempEdit == obj)
                m_ratedLoadEdit->setFocus();
            else if(m_ratedLoadEdit == obj)
                m_primaryValEdit->setFocus();
            else if(m_primaryValEdit == obj)
                m_coilLevelCombo->setFocus();
            else if(m_coilLevelCombo == obj)
                m_serialNumEdit->setFocus();
            else if(m_serialNumEdit == obj)
                this->parentWidget()->eventFilter(this,eve);
            else if(m_destTempEdit == obj)
                m_powerFactorEdit->setFocus();
            else if(m_powerFactorEdit == obj)
                m_secondValEdit->setFocus();
            else if(m_secondValEdit == obj)
                m_testCurEdit->setFocus();
            else if(m_testCurEdit == obj)
                m_coilNumEdit->setFocus();
            else if(m_coilNumEdit == obj)
                this->parentWidget()->eventFilter(this,eve);
            return true;
        case Qt::Key_Down:
            if(m_coilNumEdit == obj)
                m_testCurEdit->setFocus();
            if(m_testCurEdit == obj)
                m_secondValEdit->setFocus();
            if(m_secondValEdit == obj)
                m_powerFactorEdit->setFocus();
            if(m_powerFactorEdit == obj)
                m_destTempEdit->setFocus();
//            if(m_destTempEdit == obj)
//                focusNextChild();
            if(m_serialNumEdit == obj)
                m_coilLevelCombo->setFocus();
            if(m_coilLevelCombo == obj)
                m_primaryValEdit->setFocus();
            if(m_primaryValEdit == obj)
                m_ratedLoadEdit->setFocus();
            if(m_ratedLoadEdit == obj)
                m_testTempEdit->setFocus();
            if(m_testTempEdit == obj)
                this->parentWidget()->eventFilter(this,eve);
            return true;
        default:
            break;
        }
    }
    return false;
}

void CBaseParamSettingsWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_F1:
        event->ignore();
        break;
    case Qt::Key_Escape:
    {
        QKeyEvent *e = new QKeyEvent(event->type(),event->key(),Qt::NoModifier);
        QApplication::postEvent(this->parent(), e);
        event->ignore();
        break;
    }
    default:
        break;
    }
}

void CBaseParamSettingsWidget::initWidget()
{
    createInductorName();
    createBaseParam();
    createRatioParam();
    createResParam();
    setDefaultValue();
    setLineEditsReg();
    addEventFilter();
}

void CBaseParamSettingsWidget::onCurrentType()
{
    setDefaultValue();
}

void CBaseParamSettingsWidget::onVoltageType()
{
    setDefaultValue();
}

void CBaseParamSettingsWidget::createInductorName()
{
    groupBox_name = new QGroupBox(tr("互感器名称"), this);
    groupBox_name->resize(NAME_GROUPBOX_W, NAME_GROUPBOX_H);

    int x = 10;
    int y = 30;
    QLabel* serialNum = new QLabel(tr("编号:"), groupBox_name);
    serialNum->move(x, y);
    x += 60;
    m_serialNumEdit = new QLineEdit(groupBox_name);
    m_serialNumEdit->move(x, y-ITEM_CONTENT_OFFSET);
    m_serialNumEdit->resize(100, ITEM_WIDTH);

    x = 200;
    QLabel* coilLabel = new QLabel(tr("绕组号:"), groupBox_name);
    coilLabel->move(x, y);
    x += 60;
    m_coilNumEdit = new QLineEdit(groupBox_name);
    m_coilNumEdit->move(x, y-ITEM_CONTENT_OFFSET);
    m_coilNumEdit->resize(100, ITEM_WIDTH);
}

void CBaseParamSettingsWidget::createBaseParam()
{
    groupBox_baseParam = new QGroupBox(tr("基本参数"), this);
    groupBox_baseParam->resize(BASE_PARAM_W, BASE_PARAM_H);
    groupBox_baseParam->move(BASE_PARAM_L, BASE_PARAM_T);

    int x = 10;
    int y = 30;
    QLabel* coilLevelLabel = new QLabel(tr("绕组级别:"), groupBox_baseParam);
    coilLevelLabel->move(x, y);
    x += 80;
    m_coilLevelCombo = new MQComboBox(groupBox_baseParam);
    m_coilLevelCombo->move(x, y-ITEM_CONTENT_OFFSET);
    m_coilLevelCombo->resize(80, ITEM_WIDTH);

    MListWidget *levelListView = new MListWidget();
    m_coilLevelCombo->setModel(levelListView->model());
    m_coilLevelCombo->setView(levelListView);
    QStringList levelList;
    levelList<<tr("P")<<tr("计量")<<tr("PR")<<tr("PX")<<tr("TPX")<<tr("TPY")<<tr("TPZ")<<tr("TPS");
    m_coilLevelCombo->addItems(levelList);


    x = 200;
    QLabel* testCurLabel = new QLabel(tr("测试电流:"), groupBox_baseParam);
    testCurLabel->move(x, y);
    x += 80;
    m_testCurEdit = new QLineEdit(groupBox_baseParam);
    m_testCurEdit->move(x, y-ITEM_CONTENT_OFFSET);
    m_testCurEdit->resize(80, ITEM_WIDTH);
    x += 82;
    QLabel* testCurUnitLabel  = new QLabel(tr("A"), groupBox_baseParam);
    testCurUnitLabel->move(x, y);
    testCurUnitLabel->resize(10, ITEM_WIDTH);
}

void CBaseParamSettingsWidget::createRatioParam()
{
    groupBox_ratio = new QGroupBox(tr("变比"), this);
    groupBox_ratio->resize(RATIO_PARAM_W, RATIO_PARAM_H);
    groupBox_ratio->move(RATIO_PARAM_L, RATIO_PARAM_T);

    int x = 10;
    int y = 30;
    m_primaryValLabel = new QLabel(tr("一次电流:"), groupBox_ratio);
    m_primaryValLabel->move(x, y);
    x += 80;
    m_primaryValEdit = new QLineEdit(groupBox_ratio);
    m_primaryValEdit->move(x, y-ITEM_CONTENT_OFFSET);
    m_primaryValEdit->resize(80, ITEM_WIDTH);
    x += 82;
    m_primaryUnitLabel = new QLabel(tr("A"), groupBox_ratio);
    m_primaryUnitLabel->move(x, y);
    m_primaryUnitLabel->resize(10, ITEM_WIDTH);

    x = 200;
    m_secondValLabel = new QLabel(tr("二次电流:"), groupBox_ratio);
    m_secondValLabel->move(x, y);
    x += 80;
    m_secondValEdit = new QLineEdit(groupBox_ratio);
    m_secondValEdit->move(x, y-ITEM_CONTENT_OFFSET);
    m_secondValEdit->resize(80, ITEM_WIDTH);
    x += 82;
    m_secondUnitLabel = new QLabel(tr("A"), groupBox_ratio);
    m_secondUnitLabel->move(x, y);
    m_secondUnitLabel->resize(10, ITEM_WIDTH);


    x = 10;
    y += 30;
    QLabel* ratioLoadLabel = new QLabel(tr("额定负荷:"), groupBox_ratio);
    ratioLoadLabel->move(x, y);
    x += 80;
    m_ratedLoadEdit = new QLineEdit(groupBox_ratio);
    m_ratedLoadEdit->move(x, y-ITEM_CONTENT_OFFSET);
    m_ratedLoadEdit->resize(80, ITEM_WIDTH);
    x += 82;
    QLabel* ratioLoadUnitLabel = new QLabel(tr("VA"), groupBox_ratio);
    ratioLoadUnitLabel->move(x, y);
    ratioLoadUnitLabel->resize(20, ITEM_WIDTH);

    x = 200;
    QLabel* powerFactorLabel = new QLabel(tr("功率因素:"), groupBox_ratio);
    powerFactorLabel->move(x, y);
    x += 80;
    m_powerFactorEdit = new QLineEdit(groupBox_ratio);
    m_powerFactorEdit->move(x, y-ITEM_CONTENT_OFFSET);
    m_powerFactorEdit->resize(80, ITEM_WIDTH);
}

void CBaseParamSettingsWidget::createResParam()
{
    groupBox_res = new QGroupBox(tr("直阻"), this);
    groupBox_res->resize(RES_PARAM_W, RES_PARAM_H);
    groupBox_res->move(RES_PARAM_L, RES_PARAM_T);

    int x = 10;
    int y = 30;
    QLabel* testTempLabel = new QLabel(tr("当前温度:"), groupBox_res);
    testTempLabel->move(x, y);
    x += 80;
    m_testTempEdit = new QLineEdit(groupBox_res);
    m_testTempEdit->move(x, y-ITEM_CONTENT_OFFSET);
    m_testTempEdit->resize(78, ITEM_WIDTH);
    x += 82;
    QLabel* testTempUnitLabel = new QLabel(tr("℃"), groupBox_res);
    testTempUnitLabel->move(x, y);
    testTempUnitLabel->resize(15, ITEM_WIDTH);

    x = 200;
    QLabel* destTempLabel = new QLabel(tr("目标温度:"), groupBox_res);
    destTempLabel->move(x, y);
    x += 80;
    m_destTempEdit = new QLineEdit(groupBox_res);
    m_destTempEdit->move(x, y-ITEM_CONTENT_OFFSET);
    m_destTempEdit->resize(78, ITEM_WIDTH);
    x += 82;
    QLabel* destTempUnitLabel = new QLabel(tr("℃"), groupBox_res);
    destTempUnitLabel->move(x, y);
    destTempUnitLabel->resize(15, ITEM_WIDTH);
}

void CBaseParamSettingsWidget::setDefaultValue()
{
    if(INDUCTOR_CT == CDataManager::getInstance()->getInductorType())
    {
        CCTBaseParamSettings * ctBaseParam= CDataManager::getInstance()->ctBaseParamSettings();
        m_serialNumEdit->setText(ctBaseParam->CtNum());
        m_coilNumEdit->setText(ctBaseParam->windingNum());
        m_coilLevelCombo->setCurrentIndex(ctBaseParam->testLevel());
        m_testCurEdit->setText(QString::number(ctBaseParam->maxTestCurrent()));
        m_primaryValEdit->setText(QString::number(ctBaseParam->primaryCurrent()));
        m_secondValEdit->setText(QString::number(ctBaseParam->secondCurrent()));
        m_ratedLoadEdit->setText(QString::number(ctBaseParam->ratedLoad()));
        m_powerFactorEdit->setText(QString::number(ctBaseParam->powerFactor()));
        m_testTempEdit->setText(QString::number(ctBaseParam->currentTemp()));
        m_destTempEdit->setText(QString::number(ctBaseParam->destTemp()));
    }
    else if(INDUCTOR_PT == CDataManager::getInstance()->getInductorType()){


    }
}

#define SET_LINEEDIT_VALIDATOR(lineEdit, minVal, maxVal, decimals) \
    do {    \
        MDoubleValidator *validator = new MDoubleValidator(minVal, maxVal, decimals, this);\
        validator->setNotation(QDoubleValidator::StandardNotation); \
        lineEdit->setValidator(validator);  \
    } while(0)
void CBaseParamSettingsWidget::setLineEditsReg()
{
    SET_LINEEDIT_VALIDATOR(m_testCurEdit,     0.0,   5.0, 2);
    //SET_LINEEDIT_VALIDATOR(m_primaryValEdit,  1.0,   1000000.0, 0);
    m_primaryValEdit->setValidator(new QIntValidator(1, 1000000, this));
    SET_LINEEDIT_VALIDATOR(m_secondValEdit,   0.0,   5.0, 2);
    SET_LINEEDIT_VALIDATOR(m_ratedLoadEdit,   0.0,   1000.0, 2);
    SET_LINEEDIT_VALIDATOR(m_powerFactorEdit, 0.0,   1.0, 2);
    SET_LINEEDIT_VALIDATOR(m_testTempEdit,   -100.0, 100.0, 2);
    SET_LINEEDIT_VALIDATOR(m_destTempEdit,   -100.0, 100.0, 2);
}

void CBaseParamSettingsWidget::addEventFilter()
{
    m_serialNumEdit->installEventFilter(this);
    m_coilNumEdit->installEventFilter(this);
    m_coilLevelCombo->installEventFilter(this);
    m_testCurEdit->installEventFilter(this);
    m_primaryValEdit->installEventFilter(this);
    m_secondValEdit->installEventFilter(this);
    m_ratedLoadEdit->installEventFilter(this);
    m_powerFactorEdit->installEventFilter(this);
    m_testTempEdit->installEventFilter(this);
    m_destTempEdit->installEventFilter(this);
}
