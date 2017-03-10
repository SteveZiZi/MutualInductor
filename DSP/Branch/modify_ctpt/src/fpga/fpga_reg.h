#ifndef FPGA_REG_H
#define FPGA_REG_H

#include <stdint.h>
#include <stddef.h>


#pragma pack(push, 1)

// works for gcc
union range_set {
    struct {
        uint32_t ch1_range_value : 4;
        uint32_t ch2_range_value : 2;
        uint32_t ch3_range_value : 2;
        uint32_t ch4_range_value : 2;
    };
    uint32_t value;
};

union ping_config {
    struct {
        uint32_t frame_length       :16;
        uint32_t sample_rate_set    :3;
        uint32_t reserved           :1;
        uint32_t fix_follow_sel     :1;
        uint32_t reserved2          :11;
    };
    uint32_t value;
};

union ping_control {
    struct {
        uint32_t int_status_timeout : 1;
        uint32_t int_status_full    : 1;
        uint32_t reserved0    : 2;
        uint32_t int_enable_timeout: 1;
        uint32_t int_enable_full: 1;
        uint32_t reserved1: 2;
        uint32_t rx_ad_enable: 1;
        uint32_t reserved3: 23;
    };
    uint32_t value;
};

union pong_config {
    struct {
        uint32_t frame_length       :16;
        uint32_t sample_rate_set    :3;
        uint32_t reserved           :1;
        uint32_t fix_follow_sel     :1;
        uint32_t reserved2          :11;
    };
    uint32_t value;
};

union pong_control {
    struct {
        uint32_t int_status_timeout : 1;
        uint32_t int_status_full    : 1;
        uint32_t reserved0    : 2;
        uint32_t int_enable_timeout: 1;
        uint32_t int_enable_full: 1;
        uint32_t reserved1: 2;
        uint32_t rx_ad_enable: 1;
        uint32_t reserved3: 23;
    };
    uint32_t value;
};

struct fpga_reg
{
    uint32_t     int_en;        /*  */
    uint32_t     int_status;    /*  */
    uint32_t     dc_ac_run_status;
    uint32_t     dc_ac_start;
    uint32_t     dead_time_set;
    uint32_t     dc_dc_v200_set;   /* [0~10]: 80kHz 1250个计数值 */
    float        dc_ac_v_set;   /* 最大5.0 */
    uint32_t     dc_dc_f_set;   /* 每两个点的时间间隔，以10ns计数 */
    uint32_t     dc_ac_p_set;   /* [0~9]: DC-AC模块输出AC相位,即正弦波的相位, =1000*(360/要输出的角度) */
    //uint32_t     power_freq;    /* RO[0~26]:频率计数，基于100MHz; [31]: 1代表超出  */
    uint32_t     dc_dc_v12_set; /* [0~10]: 80kHz 1250个计数值 */
    uint32_t     utc_high;
    uint32_t     utc_low;       // 12*4 = 48 = 0x3C

    uint32_t     reserved00[4]; //

    uint32_t     fpga_version;  // 0x40
    uint32_t     output_sel;    // 1:直流源  0:交流源
    uint32_t     dac_output_settings;  // 0~0xFFFF
    uint32_t     signal_filter_times;  // 0x4C

    uint32_t     reserved01[(0x11C-0x50)/sizeof(uint32_t)];

    uint32_t     overPowerCtrlAndStatus;
    uint32_t     overPowerPower;
    union range_set  overPowerRangeSet;       //

    // AD寄存器
    uint32_t     fix_sample_count; // 0x128 固定采样率计数值

    // works for gcc
    union range_set range_set;

    uint32_t     timeout;

    uint32_t     end_addr_count_ping;
    union ping_config ping_config;    //0x138
    union ping_control ping_control;

    uint32_t     end_addr_count_pong;
    union pong_config pong_config;               // +reserved+
    union pong_control pong_control;
};
#pragma pack(pop)

uint32_t __read_reg32( uint32_t addr );
uint32_t __write_reg32( uint32_t addr, uint32_t value );
uint32_t __write_reg32( uint32_t addr, double value );


#define reg_offset(member) offsetof(struct fpga_reg, member )
#define READ_THIS_REG(m) __read_reg32( reg_offset(m) )
#define WRITE_THIS_REG(m, value) __write_reg32( reg_offset(m), value )


#endif // FPGA_REG_H
