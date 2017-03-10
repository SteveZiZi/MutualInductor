#include "adc_demo.h"
#include "src/fpga/fpga.hpp"
#include "waveshowwidget.h"
#include "src/inc/auto_guard.h"
#include "src/inc/tmutex.h"
#include "src/fpga/adcCalibration.h"
#include "src/fpga/ac_source.h"
#include "updatethread.h"

#include <QThread>
#include <QDebug>
#include <QMessageBox>

#include <assert.h>

fpga_dev fpga;
ThreadMutex s_mutex;

extern bool get_adc_ranges(int chnl, const adc_range* *ranges, range_factor* *factors, int* arrSize);

const struct adc_range * g_adcRange[4] = {NULL};
range_factor * g_adcRangeFactor[4] = {NULL};
int g_adcRangeCnt[4] = {0};

inline const QString rangeToString(const struct adc_range *range)
{
    return QString("%1(%2 -> %3)").arg(range->fpga_reg_value,2, 16, QChar('0'))
                                  .arg(range->range_min, 0, 'f', 3, QChar('0'))
                                  .arg(range->range_max, 0, 'f', 3, QChar('0'));
}

/******************************************
 * wave record
 * ****************************************/
#include <QFile>
#include <QDateTime>
#include <QException>
#define RECORD_FILE_PATH "/mnt/mmcblk0p1/"
class Recorder {
public:
    Recorder();

    bool record(const raw_wave &wave_vol, const raw_wave &wave_cur);
    bool record(const std::vector<double> &vol, const std::vector<double> &cur);
    bool createRecordFile();
    bool stopRecord();
private:
    QFile file;
};

Recorder::Recorder()
{

}

bool Recorder::record(const raw_wave &wave_vol, const raw_wave &wave_cur)
{
    if (!file.isOpen() || file.size()>=104857600) {
        createRecordFile();
    }
    if (!file.isOpen())
        return false;

    try {
        const std::vector<double> &vol = wave_vol.get_readonly_buffer();
        int size = vol.size();
        file.write(reinterpret_cast<const char*>(&size), 4);
        file.write(reinterpret_cast<const char*>(&vol[0]), size*sizeof(double));
        const std::vector<double> &cur = wave_cur.get_readonly_buffer();
        size = cur.size();
        file.write(reinterpret_cast<const char*>(&size), 4);
        file.write(reinterpret_cast<const char*>(&cur[0]), sizeof(double)*size);
    }
    catch(QException &/*ex*/)
    {
        return false;
    }

    return true;
}

bool Recorder::record(const std::vector<double> &vol, const std::vector<double> &cur)
{
    if (!file.isOpen() || file.size()>=104857600) {
        createRecordFile();
    }
    if (!file.isOpen())
        return false;

    try {
        int size = vol.size();
        file.write(reinterpret_cast<const char*>(&size), 4);
        file.write(reinterpret_cast<const char*>(&vol[0]), size*sizeof(double));

        size = cur.size();
        file.write(reinterpret_cast<const char*>(&size), 4);
        file.write(reinterpret_cast<const char*>(&cur[0]), sizeof(double)*size);
    }
    catch(QException &/*ex*/)
    {
        return false;
    }

    return true;
}

bool Recorder::createRecordFile()
{
    if (file.isOpen())
        file.close();
    QDateTime tm = QDateTime::currentDateTime();
    QString fileName = tm.toString("yyyy-MM-dd hh-mm-ss");
    file.setFileName(RECORD_FILE_PATH+fileName+".dat");
    file.open(QIODevice::ReadWrite);
    file.seek(0);

    return file.isOpen();
}

bool Recorder::stopRecord()
{
    if (file.isOpen()) {
        file.flush();
        file.close();
    }

    return true;
}

Recorder g_recorder;

/******************************************************
 * adc_demo functions
 * ***************************************************/
settingsWidget::settingsWidget(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::adc_demoClass),
    m_pos1(0),
    m_pos2(-1),
    m_bSaveRangeFactor(false),
    m_bOutput(false),
    m_isDCSource(false),
    m_bRecord(false)
{
    ui->setupUi(this);

    itemOfWave1 = LOAD_VOL_CHNL;
    itemOfWave2 = LOAD_CUR_CHNL;

    m_waveShowWidget1 = new waveShowWidget(this);
    m_waveShowWidget1->move(0, 0);
    m_waveShowWidget1->resize(400, 370);
    m_waveShowWidget1->createTrend(false);
    m_waveShowWidget2 = new waveShowWidget(this);
    m_waveShowWidget2->move(400, 0);
    m_waveShowWidget2->resize(400, 370);
    m_waveShowWidget2->createTrend(true);

    //chnl
    QStringList chnlCaptions;
    chnlCaptions << "load cur" << "src vol" << "load vol" << "pri vol";
    ui->comboBox_chnl1->addItems(chnlCaptions);
    ui->comboBox_chnl1->setCurrentIndex(itemOfWave1);
    ui->label_wave1Chnl->setText(QString("chnl%1:").arg(itemOfWave1));
    ui->comboBox_chnl2->addItems(chnlCaptions);
    ui->comboBox_chnl2->setCurrentIndex(itemOfWave2);
    ui->label_wave2Chnl->setText(QString("chnl%1:").arg(itemOfWave2));
    connect(ui->comboBox_chnl1, SIGNAL(currentIndexChanged(int)), this, SLOT(wave1Changed(int)));
    connect(ui->comboBox_chnl2, SIGNAL(currentIndexChanged(int)), this, SLOT(wave2Changed(int)));

    //source
    ui->lineEdit_output->setText("0.0");
    connect(ui->pushButton_add5, SIGNAL(clicked()), this, SLOT(add_5()));
    connect(ui->pushButton_add1, SIGNAL(clicked()), this, SLOT(add_1()));
    connect(ui->pushButton_add0p1, SIGNAL(clicked()), this, SLOT(add_0p1()));
    connect(ui->pushButton_add0p01, SIGNAL(clicked()), this, SLOT(add_0p01()));
    connect(ui->pushButton_minus5, SIGNAL(clicked()), this, SLOT(minus_5()));
    connect(ui->pushButton_minus1, SIGNAL(clicked()), this, SLOT(minus_1()));
    connect(ui->pushButton_minus0p1, SIGNAL(clicked()), this, SLOT(minus_0p1()));
    connect(ui->pushButton_minus0p01, SIGNAL(clicked()), this, SLOT(minus_0p01()));
    connect(ui->pushButton_changeSource, SIGNAL(clicked()), this, SLOT(changeSource()));
    connect(ui->pushButton_outputSwitch, SIGNAL(clicked()), this, SLOT(sourceCtrl()));

    // range set
    for(int i=0; i<MAX_CHNL_NUM; i++)
        get_adc_ranges(i, &g_adcRange[i], &g_adcRangeFactor[i], &g_adcRangeCnt[i]);
    QStringList rangeInfo;
    getRange1Infos(rangeInfo);
    ui->comboBox_chnl1Range->addItems(rangeInfo);
    connect(ui->comboBox_chnl1Range, SIGNAL(currentIndexChanged(int)), this, SLOT(range1Changed(int)));
    getRange2Infos(rangeInfo);
    ui->comboBox_chnl2Range->addItems(rangeInfo);
    connect(ui->comboBox_chnl2Range, SIGNAL(currentIndexChanged(int)), this, SLOT(range2Changed(int)));
    connect(ui->pushButton_chnl1SetRange, SIGNAL(clicked()), this, SLOT(setRange1Factor()));
    connect(ui->pushButton_chnl2SetRange, SIGNAL(clicked()), this, SLOT(setRange2Factor()));

    // pwm vpwm spwm settings
    ui->lineEdit_spwm->setText("0.0");
    ui->lineEdit_vpwm->setText("0.0");
    ui->lineEdit_pwm->setText("0.0");
    connect(ui->pushButton_setPercent, SIGNAL(clicked()), this, SLOT(setPercent()));

    // record
    connect(ui->pushButton_record, SIGNAL(clicked()), this, SLOT(btnRecord()));

    // exit
    connect(ui->pushButton_close, SIGNAL(clicked()), this, SLOT(deinit()));

    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabWidgetChanged(int)));
}

settingsWidget::~settingsWidget()
{
    delete ui;
}

void settingsWidget::Init()
{
    fpga.init(2);
    fpga.adc_start_sample();

    hInst = new UpdateThread(this, this);
    connect(hInst, SIGNAL(updateShow()), this, SLOT(updateTrend()));
    hInst->start(/*QThread::HighPriority*/);
}

void settingsWidget::updateTrend()
{
    if (m_pos2 != -1) {
        this->updateWave1Info();
        this->updateWave2Info();

        m_waveShowWidget1->updateTrend(&waves[m_pos2][itemOfWave1]);
        m_waveShowWidget2->updateTrend(&waves[m_pos2][itemOfWave2]);

        AUTO_GUARD(gd, ThreadMutex, s_mutex);
        m_pos2 = -1;
    }
}

bool settingsWidget::getWave()
{
    {
        AUTO_GUARD(gd, ThreadMutex, s_mutex);
        if (m_pos1 == m_pos2)
            m_pos1 = !m_pos2;
    }
    fpga.read(&waves[m_pos1][LOAD_CUR_CHNL],
              &waves[m_pos1][SRC_VOL_CHNL],
              &waves[m_pos1][LOAD_VOL_CHNL],
              &waves[m_pos1][PRI_VOL_CHNL]);

    if (m_bRecord) {
        g_recorder.record(waves[m_pos1][itemOfWave1], waves[m_pos1][itemOfWave2]);
    }
    else
        g_recorder.stopRecord();

    AUTO_GUARD(gd, ThreadMutex, s_mutex);
    if (m_pos2 == -1) {
        m_pos2 = m_pos1;
        return true;
    }

    return false;
}

void settingsWidget::add_5()
{
    output_add(5.0);
}
void settingsWidget::add_1()
{
    output_add(1.0);
}
void settingsWidget::add_0p1()
{
    output_add(0.1);
}
void settingsWidget::add_0p01()
{
    output_add(0.01);
}

void settingsWidget::minus_5()
{
    output_minus(5.0);
}
void settingsWidget::minus_1()
{
    output_minus(1.0);
}
void settingsWidget::minus_0p1()
{
    output_minus(0.1);
}
void settingsWidget::minus_0p01()
{
    output_minus(0.01);
}

void settingsWidget::sourceCtrl()
{
    if (m_bOutput) {
        fpga.demagnetization();
        fpga.stop_output();
        fpga.adc_start_sample();
        ui->pushButton_outputSwitch->setText(QString::fromLocal8Bit("closeSource"));
    }
    else {
        fpga.start_output();
        ui->pushButton_outputSwitch->setText(QString::fromLocal8Bit("OpenSource"));
    }
    m_bOutput = !m_bOutput;
}

void settingsWidget::changeSource()
{
    ui->lineEdit_output->setText("0.0");
    if (fpga.is_dc_source_output()) {
        fpga.change_to_ac_source();
        ui->pushButton_changeSource->setText("toDC");
    }
    else {
        fpga.change_to_dc_source();
        ui->pushButton_changeSource->setText("toAC");
    }
    fpga.start_output();
}

void settingsWidget::setPercent()
{
    if (!fpga.is_output_started())
        fpga.start_output();

    double spwm = ui->lineEdit_spwm->text().toDouble();
    double pwm = ui->lineEdit_pwm->text().toDouble();
    double vpwm = ui->lineEdit_vpwm->text().toDouble();

    fpga.acSource->dc_ac_v_set_percent(spwm);
    fpga.acSource->dc_dc_v200_set_percent(pwm);
    fpga.acSource->dc_dc_v12_set_percent(vpwm);

    ui->lineEdit_spwm->setText(QString::number(fpga.acSource->spwm_percent, 'f', 2));
    ui->lineEdit_vpwm->setText(QString::number(fpga.acSource->pwm_v12_percent, 'f', 2));
    ui->lineEdit_pwm->setText(QString::number(fpga.acSource->pwm_v200_percent, 'f', 2));
}


void settingsWidget::setRange1Factor()
{
    range_factor* factor = g_adcRangeFactor[itemOfWave1];
    int item = getRange1Item();

    factor[item].k = ui->lineEdit_chnl1K->text().toDouble();
    factor[item].b = ui->lineEdit_chnl1B->text().toDouble();

    qDebug("settingsWidget::setRange1Factor: item:%d k:%f b:%f\n", item, factor[item].k, factor[item].b);

    CAdcCalibration *calibrator = CAdcCalibration::getInstance();
    uint8_t *content = new uint8_t[256];
    ad_calibrate_result* result = reinterpret_cast<ad_calibrate_result*>(content);

    result->numOfRange = g_adcRangeCnt[itemOfWave1];
    for(uint32_t i = 0; i < result->numOfRange; i++) {
        result->factors[i].k = factor[i].k;
        result->factors[i].b = factor[i].b;
    }
    int ret = calibrator->setResult(itemOfWave1, *result);
    assert(ret==0);

    delete[] content;

    fpga.convToRealRange();
    m_bSaveRangeFactor = true;
}

void settingsWidget::setRange2Factor()
{
    range_factor* factor = g_adcRangeFactor[itemOfWave2];
    int item = getRange2Item();

    factor[item].k = ui->lineEdit_chnl2K->text().toDouble();
    factor[item].b = ui->lineEdit_chnl2B->text().toDouble();

    qDebug("settingsWidget::setRange2Factor: item:%d k:%f b:%f\n", item, factor[item].k, factor[item].b);

    CAdcCalibration *calibrator = CAdcCalibration::getInstance();
    uint8_t *content = new uint8_t[256];
    ad_calibrate_result* result = reinterpret_cast<ad_calibrate_result*>(content);

    result->numOfRange = g_adcRangeCnt[itemOfWave2];
    for(uint32_t i = 0; i < result->numOfRange; i++) {
        result->factors[i].k = factor[i].k;
        result->factors[i].b = factor[i].b;
    }
    int ret = calibrator->setResult(itemOfWave2, *result);
    assert(ret==0);

    delete[] content;

    fpga.convToRealRange();
    m_bSaveRangeFactor = true;
}

void settingsWidget::deinit()
{
    hInst->quit();
    hInst->wait(1000);

    fpga.adc_stop_sample();
    fpga.demagnetization();
    fpga.change_to_ac_source();

    if(m_bSaveRangeFactor) {
        QMessageBox::StandardButton btn = QMessageBox::question(this, "Warning", "Do you want save AD Range?");
        if (btn == QMessageBox::Yes) {
            CAdcCalibration *calibrator = CAdcCalibration::getInstance();
            calibrator->saveResult();
        }
    }

    this->close();
}

void settingsWidget::range1Changed(int index)
{
    const struct adc_range * range = g_adcRange[itemOfWave1];
    fpga.setContinueSample(false);
    if (index == 0)
        fpga.select_range_by_value(itemOfWave1, 0.00001);
    else
        fpga.select_range_by_value(itemOfWave1, range[index-1].range_max+0.01);

    updateRangeFactor1();
}

void settingsWidget::range2Changed(int index)
{
    const struct adc_range * range = g_adcRange[itemOfWave2];
    fpga.setContinueSample(false);
    if (index == 0)
        fpga.select_range_by_value(itemOfWave2, 0.00001);
    else
        fpga.select_range_by_value(itemOfWave2, range[index-1].range_max+0.01);

    updateRangeFactor2();
}

void settingsWidget::wave1Changed(int index)
{
    if (index == LOAD_CUR_CHNL) {
        m_waveShowWidget1->changeWave(true);
    }
    else {
        m_waveShowWidget1->changeWave(false);
    }
    itemOfWave1 = index;
    ui->label_wave1Chnl->setText(QString("chnl%1:").arg(itemOfWave1));

    disconnect(ui->comboBox_chnl1Range, SIGNAL(currentIndexChanged(int)), this, SLOT(range1Changed(int)));
    QStringList rangeInfo;
    getRange1Infos(rangeInfo);
    ui->comboBox_chnl1Range->clear();
    ui->comboBox_chnl1Range->addItems(rangeInfo);
    connect(ui->comboBox_chnl1Range, SIGNAL(currentIndexChanged(int)), this, SLOT(range1Changed(int)));
}
void settingsWidget::wave2Changed(int index)
{
    if (index == LOAD_CUR_CHNL) {
        m_waveShowWidget2->changeWave(true);
    }
    else {
        m_waveShowWidget2->changeWave(false);
    }
    itemOfWave2 = index;
    ui->label_wave2Chnl->setText(QString("chnl%1:").arg(itemOfWave2));

    disconnect(ui->comboBox_chnl2Range, SIGNAL(currentIndexChanged(int)), this, SLOT(range2Changed(int)));
    QStringList rangeInfo;
    getRange2Infos(rangeInfo);
    ui->comboBox_chnl2Range->clear();
    ui->comboBox_chnl2Range->addItems(rangeInfo);
    connect(ui->comboBox_chnl2Range, SIGNAL(currentIndexChanged(int)), this, SLOT(range2Changed(int)));
}

void settingsWidget::btnRecord()
{
    m_bRecord = !m_bRecord;
    if (m_bRecord) {
        ui->pushButton_record->setText("stop record");
    }
    else {
        ui->pushButton_record->setText("start record");
    }
}

void settingsWidget::updateWave1Info()
{
    raw_wave &wave = waves[m_pos2][itemOfWave1];

    wave.calculate_all();
    QString text = QString("rms:%1 ").arg(wave.get_rms(), 0, 'f', 6, QChar('0'));
    double tmp = wave.get_positive_peak() + wave.get_negative_peak();
    tmp /= 2;
    text += QString("dif:%1 ").arg(tmp, 0, 'f', 6, QChar('0'));
    text += QString("peak:%1 ").arg(wave.get_peek(), 0, 'f', 6, QChar('0'));
    ui->label_chnl1Amp->setText(text);

    text = QString("base_rms:%1 angle%2").arg(wave.get_base_rms(), 0, 'f', 6, QChar('0'))
                                         .arg(wave.get_base_angle()*180*M_1_PI, 0, 'f', 4, QChar('0'));
    ui->label_chnl1Angle->setText(text);
}

void settingsWidget::updateWave2Info()
{
    raw_wave &wave = waves[m_pos2][itemOfWave2];

    wave.calculate_all();
    QString text = QString("rms:%1 ").arg(wave.get_rms(), 0, 'f', 6, QChar('0'));
    double tmp = wave.get_positive_peak() + wave.get_negative_peak();
    tmp /= 2;
    text += QString("dif:%1 ").arg(tmp, 0, 'f', 6, QChar('0'));
    text += QString("peak:%1 ").arg(wave.get_peek(), 0, 'f', 6, QChar('0'));
    ui->label_chnl2Amp->setText(text);

    text = QString("base_rms:%1 angle%2").arg(wave.get_base_rms(), 0, 'f', 6, QChar('0'))
                                         .arg(wave.get_base_angle()*180*M_1_PI, 0, 'f', 4, QChar('0'));
    ui->label_chnl2Angle->setText(text);
}

void settingsWidget::getRange1Infos(QStringList &rangeInfo)
{
    rangeInfo.clear();

    const struct adc_range *range = g_adcRange[itemOfWave1];

    for(int i = 0; i < g_adcRangeCnt[itemOfWave1]; i++) {
        rangeInfo.push_back(rangeToString(&range[i]));
    }
}
void settingsWidget::getRange2Infos(QStringList &rangeInfo)
{
    rangeInfo.clear();

    const struct adc_range *range = g_adcRange[itemOfWave2];

    for(int i = 0; i < g_adcRangeCnt[itemOfWave2]; i++) {
        rangeInfo.push_back(rangeToString(&range[i]));
    }
}

int settingsWidget::getRange1Item()
{
    const adc_range * range = fpga.adc_range_info(static_cast<AD_CHNL_STYLE>(itemOfWave1));
    for(int i=0; i<g_adcRangeCnt[itemOfWave1]; i++)
        if (range->fpga_reg_value == g_adcRange[itemOfWave1][i].fpga_reg_value)
            return i;

    return -1;
}
int settingsWidget::getRange2Item()
{
    const adc_range * range = fpga.adc_range_info(static_cast<AD_CHNL_STYLE>(itemOfWave2));
    for(int i=0; i<g_adcRangeCnt[itemOfWave2]; i++)
        if (range->fpga_reg_value == g_adcRange[itemOfWave2][i].fpga_reg_value)
            return i;

    return -1;
}

void settingsWidget::updateRangeFactor1()
{
    const range_factor* factor = g_adcRangeFactor[itemOfWave1];
    int item = getRange1Item();

    ui->lineEdit_chnl1K->setText(QString::number(factor[item].k, 'f', 6));
    ui->lineEdit_chnl1B->setText(QString::number(factor[item].b, 'f', 6));
    ui->comboBox_chnl1Range->setCurrentIndex(item);
}
void settingsWidget::updateRangeFactor2()
{
    const range_factor* factor = g_adcRangeFactor[itemOfWave2];
    int item = getRange2Item();

    ui->lineEdit_chnl2K->setText(QString::number(factor[item].k, 'f', 6));
    ui->lineEdit_chnl2B->setText(QString::number(factor[item].b, 'f', 6));
    ui->comboBox_chnl2Range->setCurrentIndex(item);
}


void settingsWidget::tabWidgetChanged(int index)
{
    switch(index) {
    case 0:
        // chnl Amp
        break;
    case 1:
        // source output
        break;
    case 2:
        // range set
        disconnect(ui->comboBox_chnl1Range, SIGNAL(currentIndexChanged(int)), this, SLOT(range1Changed(int)));
        disconnect(ui->comboBox_chnl2Range, SIGNAL(currentIndexChanged(int)), this, SLOT(range2Changed(int)));
        ui->label_wave1Chnl_2->setText(ui->label_wave1Chnl->text());
        ui->label_wave2Chnl_2->setText(ui->label_wave2Chnl->text());
        ui->comboBox_chnl1Range->setCurrentIndex(getRange1Item());
        ui->comboBox_chnl2Range->setCurrentIndex(getRange2Item());
        updateRangeFactor1();
        updateRangeFactor2();
        connect(ui->comboBox_chnl1Range, SIGNAL(currentIndexChanged(int)), this, SLOT(range1Changed(int)));
        connect(ui->comboBox_chnl2Range, SIGNAL(currentIndexChanged(int)), this, SLOT(range2Changed(int)));
        break;
    case 3:
        // spwm pwm vpwm set
        ui->lineEdit_spwm->setText(QString::number(fpga.acSource->spwm_percent, 'f', 2));
        ui->lineEdit_vpwm->setText(QString::number(fpga.acSource->pwm_v12_percent, 'f', 2));
        ui->lineEdit_pwm->setText(QString::number(fpga.acSource->pwm_v200_percent, 'f', 2));
        break;
    case 4:
        // record
        break;
    default:
        break;
    }
}

void settingsWidget::output_add(double step)
{
    if (!fpga.is_output_started())
        fpga.start_output();

    bool ok = false;
    double vol = ui->lineEdit_output->text().toDouble(&ok);
    if (ok) {
        vol += step;
        if (vol > 100.0)
            vol = 100.0;
    }
    ui->lineEdit_output->setText(QString::number(vol, 'f', 2));
    fpga.safe_output(vol, false);
}

void settingsWidget::output_minus(double step)
{
    bool ok = false;
    double vol = ui->lineEdit_output->text().toDouble(&ok);
    if (ok) {
        vol -= step;
        if (vol < 0.0)
            vol = 0.0;
    }
    ui->lineEdit_output->setText(QString::number(vol, 'f', 2));
    fpga.safe_output(vol, false);
}


