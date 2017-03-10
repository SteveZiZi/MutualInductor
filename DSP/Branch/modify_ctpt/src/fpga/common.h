#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <string>

/*
 * 错误代码列表：高16位为硬件异常代码  低16位预留给算法层使用
 */
#define  ERR_CODE_HARDWARE_MASK 0xFFFF0000
#define  ERR_CODE_USER_MASK     0x0000FFFF

#define  NO_ERR             0x00000000
#define  ERR_SUCCESS        0x00000000

#define  ERR_SRC_VOL_AD     0x00010000
#define  ERR_LOAD_CUR_AD    0x00020000
#define  ERR_LOAD_VOL_AD    0x00040000
#define  ERR_PRI_VOL_AD     0x00080000
#define  ERR_AC_SOURCE      0x00010000
#define  ERR_DC_SOURCE      0x00020000

#define  ERR_OVER_CURRENT   0x01000000
#define  ERR_OVER_TEMP      0x02000000
#define  ERR_OVER_POWER     0x04000000

/*
 *   用户错误代码起始代码
 */
#define  ERR_CODE_USER      0x00000001


void set_error_code(uint32_t flag);  //defined in fpga.cxx
void reset_error_code();             //defined in fpga.cxx
uint32_t get_error_code();           //defined in fpga.cxx
std::string error_code_to_string(uint32_t err_code);

#endif // COMMON_H

