#include "testacsource.h"
#include "testadc.h"
#include <assert.h>

#include "src/fpga/raw_wave.hpp"
#include "src/fpga/fpga.hpp"
#include "src/fpga/adc_ctrl.h"
#include "src/fpga/ac_source.h"

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <map>


extern fpga_dev fpga;

extern  bool is_outoff_range(double v, const struct adc_range *range);

testAcSource::testAcSource(testAdc* adc) :
    m_adc(adc),
    m_freq(50.0),
    m_voltage(0)
{
    assert(m_adc);
}

void testAcSource::printMenu()
{
    //std::cout << "Source config menus:" << std::endl;

    if (fpga.is_output_started())
        printf("2 -> Enable Source Output.\n");
    else
        printf("2 -> Disable  Source Output.\n");

    if (fpga.is_dc_source_output())
        printf("3 -> Enable DC Source\n");
    else
        printf("3 -> Disable DC Source\n");

    printf("v -> set Output Voltage, current:%f\n", m_voltage);
    printf("f -> set signal Freq, current:%f\n", m_freq);

    printf("c -> Calibrate ac Source\n");
    printf("I -> Inversion configurations\n");

//    printf("e -> exit Menu.\n");
//    std::cout << "please select your menu:";
}

bool testAcSource::execMenu(char menu)
{
    switch(menu) {
    case '2':
        if (fpga.is_output_started())
            output(false);
        else
            output(true);
        break;
    case '3':
        if (fpga.is_dc_source_output())
            openDCSource(false);
        else
            openDCSource(true);
        break;
    case 'c':
        calibrateAcSource();
        break;
    case 'I':
        configInversion();
        break;
    case 'v':
        setVoltage();
        break;
    case 'f':
        setFreq();
        break;
    case 'e':
        exitMenu();
        return true;
        break;

    default:
        break;
    }

    return false;
}

void testAcSource::terminal()
{
    fpga.demagnetization();
    fpga.change_to_ac_source();
}

bool testAcSource::output(bool enable)
{
    bool cur_enable = fpga.is_output_started();

    if ( cur_enable != enable ) {
        if (enable)
            return fpga.start_output() >= 0 ? true : false;
        else
            return fpga.stop_output() >= 0 ? true : false;
    }

    return true;
}

bool testAcSource::openDCSource(bool bOpen)
{
    if (bOpen) {
        if (fpga.is_ac_source_output())
            fpga.change_to_dc_source();
    }
    else {
        if (fpga.is_dc_source_output())
            fpga.change_to_ac_source();
    }

    return true;
}

void testAcSource::setVoltage(double vol)
{
    output(true);
    m_voltage = vol;
    fpga.safe_output(vol, false);
}

void testAcSource::setFreq(double freq)
{
    output(true);
    m_freq = freq;
    fpga.dc_ac_f_set(freq);
}

void testAcSource::exitMenu()
{

}

void testAcSource::setVoltage()
{
    std::cout << "please intput double value for voltage: ";
    double vol;
    std::cin >> vol;
    setVoltage(vol);
}

void testAcSource::setFreq()
{
    std::cout << "please intput double value for freq: ";
    double freq;
    std::cin >> freq;
    setFreq(freq);
}

void testAcSource::calibrateAcSource()
{
    char sel = 'e';
    do
    {
        std::cout << "p -> calibrate pwm" << std::endl;
        std::cout << "v -> calibrate vpwm" << std::endl;
        std::cout << "s -> calibrate spwm" << std::endl;
        std::cout << "please enter a char for menu(e -> exit)" << std::endl;
        std::cin >> sel;

        switch(sel) {
        case 'p':
        case 'P':
            calibrate_pwm();
            break;
        case 'v':
        case 'V':
            calibrate_vpwm();
            break;
        case 's':
        case 'S':
            calibrate_spwm();
            break;
        default:
            sel = 'e';
            break;
        }
    }while(sel != 'e');
}

static void calc(std::map<double,double>& result)
{
    std::map<double,double>::iterator last_it = result.end();
    last_it--;
    double maxVal = last_it->second*1.414;

    for(std::map<double,double>::iterator it = result.begin(); it!=result.end();it++)
        it->second = it->second*1.414 / maxVal * 100.0;

    for(std::map<double,double>::iterator it = result.begin(); it!=result.end();it++)
    {
        double k = 0.0;
        double b = 0.0;
        if (it == result.begin()) {
            k = 0.0;
            b = it->first;
        }
        else {
            std::map<double,double>::iterator pre_it = it;
            pre_it--;
            k = (it->first-pre_it->first)/(it->second-pre_it->second);
            b = it->first-it->second*k;
        }
        printf("{%.6f, %.6f, %.6f},\n", it->second, k, b);
    }
    printf("maxVal:%.6f\n", maxVal);
}

void testAcSource::calibrate_pwm()
{
    double pwm = 1;

    fpga.set_max_range();
    fpga.setContinueSample(true);
    fpga.adc_start_sample();

    this->openDCSource(false);
    this->output(true);

    fpga.acSource->dc_dc_v12_set_percent(0);
    fpga.acSource->dc_ac_v_set_percent(100);

    std::map<double,double> result;

    bool rc = false;
    do {
     if (pwm > 100.0) {
         pwm = 100.0;
         rc = true;
     }
     fpga.acSource->dc_dc_v200_set_percent(pwm);
     double fSrcOutputVol = wait_output_steaby();
     printf("%f, %f\n", pwm, fSrcOutputVol);
     result.insert(std::make_pair(pwm, fSrcOutputVol));
     pwm += 2.0;
    }while(!rc);

    std::cout << "demagnetization" << std::endl;
    while(pwm > 0.0) {
        fpga.acSource->dc_dc_v200_set_percent(pwm);
        sleep(1);
        pwm -= 5.0;
    }
    fpga.acSource->dc_dc_v200_set_percent(0.0);
    sleep(1);
    fpga.stop_output();
    fpga.adc_stop_sample();
    fpga.set_max_range();

    calc(result);
}

void testAcSource::calibrate_vpwm()
{
    double vpwm = 1;

    fpga.setContinueSample(true);
    fpga.adc_start_sample();

    this->openDCSource(false);
    this->output(true);

    fpga.acSource->dc_dc_v200_set_percent(0);
    fpga.acSource->dc_ac_v_set_percent(100);

    std::map<double,double> result;

    bool rc = false;
    do {
     if (vpwm > 100.0) {
         vpwm = 100.0;
         rc = true;
     }
     fpga.acSource->dc_dc_v12_set_percent(vpwm);
     double fSrcOutputVol = wait_output_steaby();
     printf("%f, %f\n", vpwm, fSrcOutputVol);
     result.insert(std::make_pair(vpwm, fSrcOutputVol));
     vpwm += 2.0;
    }while(!rc);

    std::cout << "demagnetization" << std::endl;
    while(vpwm > 0.0) {
        fpga.acSource->dc_dc_v12_set_percent(vpwm);
        sleep(1);
        vpwm -= 5.0;
    }
    fpga.acSource->dc_dc_v12_set_percent(0.0);
    sleep(1);
    fpga.stop_output();
    fpga.adc_stop_sample();
    fpga.set_max_range();

    calc(result);
}

void testAcSource::calibrate_spwm()
{
    double percent = 1;

    fpga.setContinueSample(true);
    fpga.adc_start_sample();

    this->openDCSource(false);
    this->output(true);

    fpga.acSource->dc_dc_v200_set_percent(50);
    fpga.acSource->dc_dc_v12_set_percent(0);
    fpga.acSource->dc_ac_v_set_percent(0);

    std::map<double,double> result;

    bool rc = false;
    do {
     if (percent > 100.0) {
         percent = 100.0;
         rc = true;
     }
     fpga.acSource->dc_ac_v_set_percent(percent);
     double fSrcOutputVol = wait_output_steaby();
     printf("%f, %f\n", percent, fSrcOutputVol);
     result.insert(std::make_pair(percent, fSrcOutputVol));
     percent += 2.0;
    }while(!rc);

    std::cout << "demagnetization" << std::endl;
    percent = 50.0;
    while(percent > 0.0) {
        fpga.acSource->dc_dc_v200_set_percent(percent);
        sleep(1);
        percent -= 5.0;
    }
    fpga.acSource->dc_dc_v200_set_percent(0.0);
    fpga.acSource->dc_ac_v_set_percent(0);
    sleep(1);
    fpga.stop_output();
    fpga.adc_stop_sample();
    fpga.set_max_range();

    calc(result);
}

double testAcSource::wait_output_steaby(bool */*ok*/)
{
    bool rc = false;
    int ncnt = 0;
    double fSrcOutputVol = 0.0;
    raw_wave ch2;               // src voltage

    do {
        int flag = 0;

        if ( -1 == fpga.read( 0, &ch2, 0, 0) ) {
            break;
        }
        ch2.calculate_all();
        double positive_peak = fabs(ch2.get_positive_peak());
        double negative_peak = fabs(ch2.get_negative_peak());
        double peak = (positive_peak > negative_peak) ? positive_peak : negative_peak;

        if (is_outoff_range(peak, fpga.adc_range_info(SRC_VOL_CHNL))) {
            fpga.select_range_by_value(SRC_VOL_CHNL, peak);
            flag = 1;
        }
        if (flag) {
            fpga.ignoreADSampleValue(4);
            fSrcOutputVol = ch2.get_rms();
        }
        else {
            if (fSrcOutputVol > 50.0) {
                if ((fabs(ch2.get_rms()-fSrcOutputVol)/fSrcOutputVol) < 0.005)
                    rc = true;
            }
            else {
                if (fabs(ch2.get_rms() - fSrcOutputVol) < 0.002)
                    rc = true;
            }
            fSrcOutputVol = ch2.get_rms();
            ncnt++;
        }
    }while(!rc&&ncnt < 200);

    return fSrcOutputVol;
}

void testAcSource::configInversion()
{
    static double pwm = 0.0;
    static double vpwm = 0.0;
    static double spwm = 0.0;

    if (fpga.is_output_started()) {
        if (fpga.is_dc_source_output())
            fpga.change_to_ac_source();
        //calc_percent(m_voltage, pwm, vpwm, spwm);
    }
    else {
        fpga.change_to_ac_source();
        fpga.start_output();
    }
    fpga.acSource->dc_ac_v_set_percent(spwm);
    fpga.acSource->dc_dc_v200_set_percent(pwm);
    fpga.acSource->dc_dc_v12_set_percent(vpwm);

    char sel = 'e';
    do
    {
        std::cout << "Inversion config Menus:" << std::endl;
        std::cout << "    p -> pwm percent:" << pwm << std::endl;
        std::cout << "    v -> vpwm percent" << vpwm << std::endl;
        std::cout << "    s -> spwm percent" << spwm << std::endl;
        std::cout << "    e -> exit" << std::endl;
        std::cout << "please enter a Char and Percent Value" << std::endl;
        std::cin >> sel;

        switch(sel) {
        case 'p':
        case 'P':
            std::cin >> pwm;
            fpga.acSource->dc_dc_v200_to_percent(pwm);
            break;
        case 'v':
        case 'V':
            std::cin >> vpwm;
            fpga.acSource->dc_dc_v12_to_percent(vpwm);
            break;
        case 's':
        case 'S':
            std::cin >> spwm;
            fpga.acSource->dc_ac_v_set_percent(spwm);
            break;
        default:
            sel = 'e';
            break;
        }
    }while(sel != 'e');
}
