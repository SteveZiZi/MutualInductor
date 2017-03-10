#include "testadc.h"
#include "src/fpga/raw_wave.hpp"
#include "src/fpga/fpga.hpp"

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>

extern fpga_dev fpga;

extern  bool is_outoff_range(double v, const struct adc_range *range);



testAdc::testAdc() :
    m_bExitThread(false),
    m_bPauseRead(false),
    m_bAutoSelectedRange(false),
    m_bSaveWave(false),
    m_bConfigAdc(false),
    m_bEnableAdc(false),
    m_mask(0x0F),
    m_cycleCount(2),
    m_sampleCount(1024),
    m_freq(50.0)
{
    fpga.setContinueSample(true);
    fpga.setCycleCount(m_cycleCount);
    fpga.set_np_per_cycle(m_sampleCount);
    fpga.dc_ac_f_set(m_freq);

    sem_init(&this->sem, 0, 0);
    pthread_t tid = 0;
    pthread_create( &tid, NULL, (void* (*)(void*))read_thread, this);
    std::cout << __func__ << " :inited tid ="<< tid << std::endl;
    assert(tid);
}

void testAdc::read_thread(testAdc *p_this)
{
    std::cout << __func__ << " started" << std::endl;

    while(!p_this->m_bExitThread)
    {
        if (p_this->m_bConfigAdc) {
            p_this->m_bPauseRead = true;
            sem_post(&p_this->sem);
            p_this->m_bConfigAdc = false;
        }
        if (p_this->m_bPauseRead) {
            usleep(50000);
            continue;
        }
        else {
            if (p_this->m_bEnableAdc && fpga.is_sample_started())
                p_this->get_rms();
            else
                usleep(50000);
        }
    }

    sem_post(&p_this->sem);
    std::cout << __func__ << " exit" << std::endl;
}

void testAdc::configMenu()
{
    m_bConfigAdc = true;
    sem_wait(&this->sem);
}

void testAdc::printMenu()
{
    std::cout << "AD config menus:" << std::endl;

    printf("1 -> Set Range for AD Channel\n");
    printf("2 -> Enable AD Chnl(current:0x%x)\n", m_mask);

    if (fpga.is_sample_started())
        printf("3 -> Enable ADC Sample.\n");
    else
        printf("3 -> Disable ADC Sample.\n");

    if (m_bAutoSelectedRange)
        printf("4 -> Enable Auto Select Range\n");
    else
        printf("4 -> Disable Auto Select Range\n");

    if (fpga.isContinueSample())
        printf("5 -> Enable Sample Thread\n");
    else
        printf("5 -> Disable Sample Thread\n");

    if (m_bSaveWave)
        printf("7 -> Save Sample Wave\n");
    else
        printf("7 -> Don't' Save Sample Wave\n");

    printf("c -> set ADC Cycle Counts, current:%d\n", m_cycleCount);
    printf("s -> set ADC Sample Counts, current:%d\n", m_sampleCount);
    printf("f -> set signale freq, current:%f\n", m_freq);

    printf("e -> exit Menu.\n");
    std::cout << "please select your menu:";
}

bool testAdc::execMenu(char menu)
{
    switch(menu) {
    case '1':
        setRange();
        break;
    case '2':
        enableADChnl();
        break;
    case '3':
        if (fpga.is_sample_started())
            fpga.adc_stop_sample();
        else
            fpga.adc_start_sample();
        break;
    case '4':
        m_bAutoSelectedRange = !m_bAutoSelectedRange;
        break;
    case '5':
        if (fpga.isContinueSample())
            fpga.setContinueSample(false);
        else
            fpga.setContinueSample(true);
        break;
    case '7':
        m_bSaveWave = !m_bSaveWave;
        break;

    case 'c':
        setCycleCount();
        break;
    case 's':
        setSampleCount();
        break;
    case 'f':
        setFreq();
        break;

    case 'e':
        exitMenu();
        return true;
        break;
    }

    return false;
}

void testAdc::terminalThread()
{
    if (!m_bExitThread) {
        m_bExitThread = true;
        sem_wait(&this->sem);
    }

    fpga.set_max_range();
}

void testAdc::setAutoSelectRange(bool autoSelect)
{
    m_bAutoSelectedRange = autoSelect;
    if (m_bAutoSelectedRange) {
        fpga.setContinueSample(false);
    }
}

void testAdc::setCycleCount(int cycleCount)
{
    m_cycleCount = cycleCount;
    fpga.setCycleCount(cycleCount);
}
void testAdc::setSampleCount(int sampleCount)
{
    m_sampleCount = sampleCount;
    fpga.set_np_per_cycle(sampleCount);
}
void testAdc::setFreq(double freq)
{
    m_freq = freq;
    fpga.dc_ac_f_set(freq);
}

void testAdc::exitMenu()
{
    if (fpga.is_sample_started()) {
        std::cout << __func__ << " start adc" << std::endl;
        setAutoSelectRange(m_bAutoSelectedRange);
        m_bEnableAdc = true;
    }
    else {
        m_bEnableAdc = false;
    }

    m_bPauseRead = false;
    std::cout << __func__ << std::endl;
}


void testAdc::saveWave(raw_wave *wave_i_load, raw_wave *wave_u_src, raw_wave *wave_u_load, raw_wave *wave_u_pri)
{
    int n = wave_i_load->get_np();

    std::vector<double> & chd1= wave_i_load->get_writable_buffer();
    std::vector<double> & chd2= wave_u_src->get_writable_buffer();
    std::vector<double> & chd3 = wave_u_load->get_writable_buffer();
    std::vector<double> & chd4 = wave_u_pri->get_writable_buffer();

    const char *filename = "./vol.csv";
    FILE *fp = fopen(filename, "w");
    if (fp){
        fprintf(fp, "src_i,src_v,sec_v,pri_v\n");
        for(int i = 0; i < n; i++)
        {
            fprintf(fp, "%f,%f,%f,%f\n", chd1[i], chd2[i], chd3[i], chd4[i]);
        }
        fclose(fp);
    }
}


void testAdc::get_rms()
{
    raw_wave wave[4];
    if(0 != fpga.read(&wave[0], &wave[1], &wave[2], &wave[3])) {
        printf("fpga.read failed\n");
        return;
    }

    if (m_bSaveWave) {
        saveWave(&wave[0], &wave[1], &wave[2], &wave[3]);
    }

    bool changeRange = false;
    for(int chnl = 0; chnl < MAX_CHNL_NUM; chnl++)
    {
        if ((m_mask&(1 << chnl)) != (1<<chnl)) {
            continue;
        }
        wave[chnl].calculate_all();

        float max = wave[chnl].get_positive_peak();
        float min = wave[chnl].get_negative_peak();
        printf("chnl[%d]-> rms:%f  base_rms:%f  thd:%f  max:%f min:%f offset:%f range_reg:%d\n",
               chnl, wave[chnl].get_rms(), wave[chnl].get_base_rms(),
               wave[chnl].get_thd(), max, min, (max+min)/2,
               fpga.adc_range_info(static_cast<AD_CHNL_STYLE>(chnl))->fpga_reg_value);

        if (m_bAutoSelectedRange) {
            double positive_peak = fabs(max);
            double negative_peak = fabs(min);
            double peak = (positive_peak > negative_peak) ? positive_peak : negative_peak;
            if (is_outoff_range(peak, fpga.adc_range_info(static_cast<AD_CHNL_STYLE>(chnl)))) {
                fpga.select_range_by_value(chnl, peak);
                changeRange = true;
            }
        }

    }

    if (fpga.is_ac_source_output())
    {
        wave[0].calculate_all();
        wave[1].calculate_all();
        double angle_cur = wave[0].get_base_angle()*180*M_1_PI;
        double angle_vol = wave[1].get_base_angle()*180*M_1_PI;
        printf("angle_cur:%f angle_vol:%f  angle_dif:%f\n", angle_cur, angle_vol, angle_cur-angle_vol);
    }
    else {
        wave[0].calculate_rms();
        wave[2].calculate_rms();
        printf("Res:%f\n", wave[2].get_rms()/wave[0].get_rms());
    }

    if (m_bAutoSelectedRange && changeRange)
    {
        int n = 0;
        double rms[4];
        while(n < 2) {
            fpga.read(&wave[0], &wave[1], &wave[2], &wave[3]);
            for(int i = 0; i < 4; i++) {
                wave[i].calculate_all();
                rms[i] = wave[i].get_rms();
            }
            printf("rms: %6.4f  %6.4f  %6.4f  %6.4f \n", rms[0], rms[1], rms[2], rms[3]);
            usleep(50000);
            n++;
        }
    }
    printf("\n");
}

void testAdc::setRange()
{
    if (m_bAutoSelectedRange) {
        setAutoSelectRange(false);
    }
    if (fpga.isContinueSample()) {
        fpga.setContinueSample(false);
    }

    for(int chnl = 0; chnl < MAX_CHNL_NUM; chnl++) {
        printRange(chnl);
    }

    int chnl;
    float val;
    std::cout << "please enter chnl num and vol/cur rms value:";
    std::cin >> chnl >> val;

    if (chnl < 0 || chnl >= MAX_CHNL_NUM) {
        std::cout << "AD Channel Error" << std::endl;
    }
    else {
        fpga.select_range_by_value(chnl, val);
        printRange(chnl);
    }
}

void testAdc::printRange(int chnl)
{
    if (chnl == LOAD_CUR_CHNL) {
        const struct adc_range* range = fpga.adc_range_info(LOAD_CUR_CHNL);
        printf("%d: load Cur (reg:0x%x range_min:%6.4f range_max:%6.4f scale:%6.4f\n",
               chnl, range->fpga_reg_value, range->range_min, range->range_max, range->scale);
    }
    else if (chnl == SRC_VOL_CHNL) {
        const struct adc_range* range = fpga.adc_range_info(SRC_VOL_CHNL);
        printf("%d: src Vol (reg:0x%x range_min:%6.4f range_max:%6.4f scale:%6.4f \n",
               chnl, range->fpga_reg_value, range->range_min, range->range_max, range->scale);
    }
    else if (chnl == PRI_VOL_CHNL) {
        const struct adc_range* range = fpga.adc_range_info(PRI_VOL_CHNL);
        printf("%d: pri Vol (reg:0x%x range_min:%6.4f range_max:%6.4f scale:%6.4f\n",
               chnl, range->fpga_reg_value, range->range_min, range->range_max, range->scale);
    }
    else if (chnl == LOAD_VOL_CHNL) {
        const struct adc_range* range = fpga.adc_range_info(LOAD_VOL_CHNL);
        printf("%d: load Vol (reg:0x%x range_min:%6.4f range_max:%6.4f scale:%6.4f\n",
               chnl, range->fpga_reg_value, range->range_min, range->range_max, range->scale);
    }
}

void testAdc::enableADChnl()
{
    std::cout << "Please Input hex Number for AD Channel mask:" << std::endl;
    int mask = 0;
    scanf("%x", &mask);

    m_mask = mask;
    printf("AD Channel mask:0x%x\n", m_mask);
}

void testAdc::setCycleCount()
{
    std::cout << "please input number for cycleCount:";
    int val;
    std::cin >> val;
    setCycleCount(val);
}
void testAdc::setSampleCount()
{
    std::cout << "please input number for sampleCount:";
    int val;
    std::cin >> val;
    setSampleCount(val);
}
void testAdc::setFreq()
{
    std::cout << "please input number for freq:";
    double freq;
    std::cin >> freq;
    setFreq(freq);
}

