#include "fpga_reg.h"
#include <stdio.h>


extern "C" {
    int open_smc(const char* dev);
    int close_smc(int fd);
    int read_smc_dev_reg(unsigned int reg, unsigned int *val);
    int write_smc_dev_reg(unsigned int reg, unsigned int val);
    int read_smc_dev_sreg(unsigned int reg,unsigned int *val);
}

//FILE *g_log = stdout;


uint32_t __read_reg32( uint32_t addr )
{
    static uint32_t  ad_addr = reg_offset(fix_sample_count);

    uint32_t value = 0;
    int ret;

    if (addr >= ad_addr){
        ret = read_smc_dev_sreg( addr, &value );
    } else {
        ret = read_smc_dev_reg( addr, &value );
    }
    if ( ret != 0 ){
        printf("read addr 0x%X error\n", addr );
    }
    //CT_LOG(("read addr 0x%x: %x(%u)\n", addr, value, value));
    return value;
}

uint32_t __write_reg32( uint32_t addr, uint32_t value )
{
    int ret = write_smc_dev_reg( (unsigned int)addr, value );
    //uint32_t r = __read_reg32(addr);
    //CT_LOG(("write addr 0x%x with 0x%x(%u), read=%x %s\n", addr, value, value, r, ((value==r&&ret==0)?"ok":"ERROR") ));
    //(void)r;
    return ret;
}

uint32_t __write_reg32( uint32_t addr, double value )
{
    float v = (float)value;
    uint32_t *p = (uint32_t*)&v;
    //CT_LOG(("write addr 0x%x with float 0x%x(%f) ", addr, *(int*)&value, value ));
    return write_smc_dev_reg( (unsigned int)addr, *p );
}

