#ifndef PRIVATE_DEF_H
#define PRIVATE_DEF_H

#include <stdint.h>
#include <math.h>

#ifndef NO_LOG
#define CT_LOG(x) printf x
#else
#define CT_LOG(x) do{}while(0)
#endif


#define TEST_MODE       0
#if TEST_MODE
#define DC_ORIGIN_100  100
#define MAX_DC_OUTPUT (100)    //  测试版本，AD采样板当前最大量程183*0.68
#else
#define MAX_DC_OUTPUT (200)
#endif

#define MAX_PEEK_POWER  (400)

#include "common.h"

enum AD_CHNL_STYLE {
    LOAD_CUR_CHNL,
    SRC_VOL_CHNL,
    SECOND_VOL_CHNL,
    LOAD_VOL_CHNL = SECOND_VOL_CHNL,
    PRI_VOL_CHNL,
    MAX_CHNL_NUM
};

enum {
    AC_SOURCE_OUTPUT,    //  交流源输出
    DC_SOURCE_OUTPUT     //  直流源输出
};


struct adc_range
{
    int fpga_reg_value;
    double range_min;
    double range_max;
    double scale;
    double line_offset;    // 线性偏执
};

struct range_factor {
    float k;
    float b;
};

struct ad_calibrate_result {
    uint32_t numOfRange;
    range_factor factors[1];  // 最多8个量程挡
};


#endif // PRIVATE_DEF_H

