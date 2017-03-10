#ifndef TESTACSOURCE_H
#define TESTACSOURCE_H

class testAdc;

class testAcSource
{
public:
    testAcSource(testAdc* adc);

    void printMenu();
    bool execMenu(char menu);
    void terminal(void);

    bool output(bool enable);
    bool openDCSource(bool bOpen);
    void setVoltage(double vol);
    void setFreq(double freq);

    void exitMenu();

private:
    void setVoltage(void);
    void setFreq();

    void calibrateAcSource();
    void calibrate_pwm();
    void calibrate_vpwm();
    void calibrate_spwm();
    double wait_output_steaby(bool *ok=0);

    void configInversion();


private:
    testAdc *m_adc;

    double m_freq;
    double m_voltage;

};

#endif // TESTACSOURCE_H
