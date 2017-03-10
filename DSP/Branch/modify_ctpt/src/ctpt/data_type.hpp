#ifndef DATA_TYPE_H
#define DATA_TYPE_H

#include <vector>
#include "src/fpga/common.h"

struct auto_scan_result
{
    double preset_freq;
    double preset_vol_start;
    double preset_vol_step;
    double preset_vol_stop;
    double preset_current_limit;
    std::vector<raw_wave> wave_vol_primary; // 初级线圈电压
    std::vector<raw_wave> wave_cur_primary; // 初级线圈电
    std::vector<raw_wave> wave_vol;
    std::vector<raw_wave> wave_cur;
};

enum USER_ERR_CODE_ENUM {
    ERR_NOT_FOUND__KNEE_POINT = ERR_CODE_USER,
    ERR_WIRE_CONNECT_WRONG
};




#endif // end DATA_TYPE_H
