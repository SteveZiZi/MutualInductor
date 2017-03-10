
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "src/fpga/fpga.hpp"
#include "src/fpga/raw_wave.hpp"
#include <stdio.h>
#include <vector>
#include <time.h>
#include <assert.h>

#include<fstream>

#include "src/fpga/fpga_reg.h"
#include "src/fpga/adc_ctrl.h"
#include "src/fpga/ac_source.h"

#include "testadc.h"
#include "testacsource.h"
#include "eeprom.h"

#if 0
#define _rms_()  get_base_rms()
#else
#define _rms_()  get_rms()
#endif

#define EN_IIR_FILTER 0

extern  bool is_outoff_range(double v, const struct adc_range *range);


#define SHF(n) printf(#n "=%f\n", n)
#define SHI(n) printf(#n "=%d\n", n)

fpga_dev fpga;

static void at_exit1(void)
{
    printf("at_exit stopped.\n");
    fpga.stop_output();
    fpga.adc_stop_sample();

    fflush(stdout);
}

class CUserTest {
public:
    CUserTest();
    ~CUserTest();

    bool isExitMenu() {return m_bExitMenu;}

    void printMenu();
    bool exeMenu(char menuItem);
    void displayResult();

private:
    void configAdc();
    void exitMenu();
    void test_eeprom();
    
private:
    bool m_bExitMenu;
    testAdc * m_adc;
    testAcSource *m_acSource;

};

CUserTest::CUserTest()
    : m_bExitMenu(false)
{
    fpga.init( 2 );
    m_adc = new testAdc();
    m_acSource = new testAcSource(m_adc);
}

CUserTest::~CUserTest()
{

}

void CUserTest::printMenu()
{
    std::cout << "Main menus:" << std::endl;
    std::cout << "1 -> AD config Menu" << std::endl;
    std::cout << "t -> test eeprom" << std::endl;

    m_acSource->printMenu();


    std::cout << "e -> exit Menu and Finish" << std::endl;
    std::cout << "please select your menu:";
}

bool CUserTest::exeMenu(char menuItem)
{
    m_adc->configMenu();

    switch(menuItem)  {
    case '1':
        configAdc();
        break;
    case 't':
        test_eeprom();
    case 'e':
        exitMenu();
        break;
    default:
        m_acSource->execMenu(menuItem);
        break;
    }
    m_adc->setPauseRead(false);
    return true;
}

void CUserTest::configAdc()
{
    char menu = -1;
    do {
        m_adc->printMenu();
        std::cin >> menu;
    }while(!m_adc->execMenu(menu));
}

void CUserTest::exitMenu()
{
    m_bExitMenu = true;
    m_acSource->terminal();
    m_adc->terminalThread();
}


#define DEV_A_NODE   "/sys/class/i2c-dev/i2c-1/device/1-0050/eeprom"
#define DEV_B_NODE   "/sys/class/i2c-dev/i2c-1/device/1-0052/eeprom"
void CUserTest::test_eeprom()
{

    time_t timep = time(NULL);
    srand((int)(timep));
    uint8_t rdBuf[256];
    uint8_t wrBuf[256];

reTest:
    std::cout << "Please input test eeprom item(A or B)";
    char item;
    std::cin >>item;

    eeprom dev;
    if (item == 'a' || item == 'A') {
        if (dev.open_dev(DEV_A_NODE) < 0)
            std::cout << "devA.open_dev failed----" << std::endl;
    }
    else if (item == 'b' || item == 'B') {
        if (dev.open_dev(DEV_B_NODE) < 0)
            std::cout << "devB.open_dev failed----" << std::endl;
    }
    else {
        goto reTest;
    }

    for(int i = 0; i < 2; i++)
    {
        for(int n = 0; n < 256; n++)
            wrBuf[n] = rand() % 256;
        std::cout << "write test" << std::endl;
        assert(dev.write_dev(256, wrBuf) == 256);
        usleep(1000);
        std::cout << "read test" << std::endl;
        assert(dev.read_dev(256, rdBuf) == 256);

        std::cout << "verify data" << std::endl;
        for(int n = 0; n < 256; n++)
            assert(wrBuf[n] == rdBuf[n]);

        std::cout << "test ok;" << std::endl;
    }
    dev.close_dev();
    goto reTest;
}

void user_test()
{
    printf("begin test\n");

    char menu = -1;
    CUserTest tester;

    while(!tester.isExitMenu()) {
        do {
            tester.printMenu();
            std::cin >> menu;
        }while(!tester.exeMenu(menu));
    }

    printf("end test\n");
}

int main(int /*argc*/, char */*argv*/[])
{
    atexit(at_exit1);

    user_test();

    fflush( stdout );

    return 0;
}
