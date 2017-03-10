#ifndef TESTADC_H
#define TESTADC_H

#include <stdint.h>
#include <semaphore.h>

class raw_wave;

class testAdc
{
public:
    testAdc();

    static void read_thread(testAdc *p_this);

    void configMenu();
    void printMenu();
    bool execMenu(char menu);
    void terminalThread();

    void setAutoSelectRange(bool autoSelect);

    void setCycleCount(int cycleCount);
    void setSampleCount(int sampleCount);
    void setFreq(double freq);

    void setPauseRead(bool pauseRead) {m_bPauseRead = pauseRead;}

private:
    void exitMenu();
    void saveWave(raw_wave *wave_i_load,raw_wave *wave_u_src,raw_wave *wave_u_load,raw_wave *wave_u_pri);
    void get_rms();

    void setRange();
    void printRange(int chnl);
    void enableADChnl();

    void setCycleCount();
    void setSampleCount();
    void setFreq();

private:
    bool m_bExitThread;
    bool m_bPauseRead;
    bool m_bAutoSelectedRange;
    bool m_bSaveWave;
    bool m_bConfigAdc;
    bool m_bEnableAdc;

    uint16_t m_mask;
    int m_cycleCount;
    int m_sampleCount;
    double m_freq;


    sem_t sem;

};

#endif // TESTADC_H
