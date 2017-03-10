#ifndef KEYBOARDDEMO_H
#define KEYBOARDDEMO_H

#include <QDialog>
#include "ui_adc_demo.h"
#include "src/fpga/raw_wave.hpp"


class waveShowWidget;
class UpdateThread;
class raw_wave;
class settingsWidget : public QDialog
{
    Q_OBJECT

public:
    settingsWidget(QWidget *parent = 0);
    ~settingsWidget();

    void Init();
    bool getWave();

private slots:
    void add_5();
    void add_1();
    void add_0p1();
    void add_0p01();
    void minus_5();
    void minus_1();
    void minus_0p1();
    void minus_0p01();
    void sourceCtrl();
    void changeSource();

    void setPercent(void);

    void updateTrend();

    void setRange1Factor();
    void setRange2Factor();

    void deinit();

    void range1Changed(int index);
    void range2Changed(int index);

    void wave1Changed(int index);
    void wave2Changed(int index);

    void btnRecord();

    void tabWidgetChanged(int index);


private:
    void output_add(double step);
    void output_minus(double step);

    void updateWave1Info(void);
    void updateWave2Info(void);

    void getRange1Infos(QStringList &rangeInfo);
    void getRange2Infos(QStringList &rangeInfo);

    int getRange1Item();
    int getRange2Item();

    void updateRangeFactor1();
    void updateRangeFactor2();

    
private:
    Ui::adc_demoClass *ui;

    waveShowWidget *m_waveShowWidget1;
    waveShowWidget *m_waveShowWidget2;
    int itemOfWave1;
    int itemOfWave2;

    UpdateThread* hInst;
    raw_wave waves[2][4];
    int m_pos1;
    int m_pos2;

    bool m_bSaveRangeFactor;
    bool m_bOutput;
    bool m_isDCSource;
    bool m_bRecord;
};

#endif // KEYBOARDDEMO_H
