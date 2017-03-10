#include "dc_source.h"
#include "fpga_reg.h"
#include "private_def.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>


#define vol_to_reg(vol) (uint32_t)((vol)/5.0*65535.0)
#define reg_to_vol(reg) ((double)(reg)*5.0/65535.0)

dc_source::dc_source() :
    outputStarted_(false),
    output_vol(0.0)
{
    outputVol(output_vol);
}

int dc_source::startOutput()
{
    outputVol(0);
    outputStarted_ = true;
    return 0;
}

int dc_source::stopOutput()
{
    outputStarted_ = false;
    return 0;
}

int dc_source::safe_output(double v)
{
    double offset = v - this->output_vol;
    double step = offset / 10;

    if (fabs(step) >= 0.01) {
        double tmp = this->output_vol;
        for(int i= 0; i < 10; i++) {
            tmp += step;
            this->outputVol(tmp);
            usleep(100000);
        }
    }
    this->outputVol(v);
    usleep(100000);

    return 0;
}

int dc_source::demagnetization()
{
    if (this->output_vol < 0.0001)
        return 0;

    do {
        double v = this->output_vol-1.0;
        if (v>0.0)
            safe_output(v);
        else
            break;
    }while(1);
    safe_output(0.0);
    return 0;
}

int dc_source::outputVol(double v)
{
    uint32_t reg = vol_to_reg(v);
    if (reg > 0xFFFF) {
        reg = 0xFFFF;
    }
    this->output_vol = reg_to_vol(reg);
    std::cout << "DC:" << v << "  reg:" << reg << std::endl;
    return WRITE_THIS_REG( dac_output_settings, reg );
}

