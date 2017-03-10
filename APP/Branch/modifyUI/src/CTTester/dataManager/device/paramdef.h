#ifndef PARAMDEF
#define PARAMDEF
#include <QString>

#include "ctpt_def.h"

enum INDUCTOR_TYPE_ENUM {
    INDUCTOR_CT,
    INDUCTOR_PT
};

enum TEST_ITEM_ENUM {
    TEST_NONE       = 0x00,
    TEST_RESISTANCE = 0x01,                 // 直阻
    TEST_EXCITATION = 0x02,                 // 励磁
    TEST_RATIO      = 0x04,                 // 变比
    TEST_BURDEN     = 0x08,                 // 负载

    TEST_DEMAGNETIOZATION = 0x1000          // 消磁
};

enum CTPT_CLASS_ENUM
{
    CTPT_P       = CTPT::CTC_P,
    CTPT_MEASURE = CTPT::CTC_MEASURE,
    CTPT_PR      = CTPT::CTC_PR,
    CTPT_PX      = CTPT::CTC_PX,
    CTPT_TPX     = CTPT::CTC_TPX,
    CTPT_TPY     = CTPT::CTC_TPY,
    CTPT_TPZ     = CTPT::CTC_TPZ,
    CTPT_TPS     = CTPT::CTC_TPS,
};

struct S_CT_Base_Param
{
    QString serialNumber;                   // 编号
    QString winding;                        // 绕阻号

    CTPT_CLASS_ENUM level;                  // 级别
    double  testCurrent;                    // 最大测试电流
    double  primaryCurrent;                 // 额定一次电流
    double  secondCurrent;                  // 额定二次电流

    double  ratedLoad;                      // 额定负荷
    double  powerFactor;                    // 功率因数

    double  currentTemp;                    // 参考温度
    double  destTemp;                       // 目标温度
};

struct S_CT_Measure_Extend_Param
{
    double  fs;                             // 额定仪表保安系数
    double  extendCurrent;                  // 扩展电流标定
};

struct S_CT_P_Extend_Param
{
    double  alf;                            // 额定准确限值系数
};

struct S_CT_PR_Extend_Param
{
    double  alf;                            // 额定准确限值系数
    double  Ts;                             // 二次时间常数
};

struct S_CT_PX_Extend_Param
{
    double  Kx;                             // 额定计算系数Kx
    double  Ek;                             // 额定拐点电势
    double  Ie;                             // Ek对应Ie
};

struct S_CT_TPX_Extend_Param
{
    double  kssc;                           // 额定对称短路电流系数
    double  ktd;                            // 暂态面积系数
    double  Tp;                             // 一次时间常数
    double  Ts;                             // 二次时间常数
    double  t1;                             // t1
    double  tal1;                           // tal1
    int     coc;                            // 工作循环
    double  tfr;                            // tfr
    double  t2;                             // t2
    double  tal2;                           // tal2
};
typedef struct S_CT_TPX_Extend_Param S_CT_TPY_Extend_Param;

struct S_CT_TPZ_Extend_Param
{
    double  kssc;                           // 额定对称短路电流系数
    double  ktd;                            // 暂态面积系数
    double  Tp;                             // 一次时间常数
    double  Ts;                             // 二次时间常数
};

struct S_CT_TPS_Extend_Param
{
    double  kssc;                           // 额定对称短路电流系数
    double  ktd;                            // 面积系数Kx
    double  Ual;                            // 额定ual
    double  Ial;                            // Ual对应最大Ia1
};

struct S_CT_Param
{
    S_CT_Base_Param                 baseParam;
    union
    {
        S_CT_Measure_Extend_Param   measureLevelParam;
        S_CT_P_Extend_Param         pLevelParam;
        S_CT_PR_Extend_Param        prLevelParam;
        S_CT_PX_Extend_Param        pxLevelParam;
        S_CT_TPX_Extend_Param       tpxLevelParam;
        S_CT_TPY_Extend_Param       tpyLevelParam;
        S_CT_TPZ_Extend_Param       tpzLevelParam;
        S_CT_TPS_Extend_Param       tpsLevelParam;
    }extend;
};

struct S_PT_Base_Param {
    QString number;                         // 编号
    QString winding;                        // 绕阻号

    CTPT_CLASS_ENUM level;                  // 级别
    double  primaryVol;                     // 额定一次电压
    double  secondVol;                      // 额定二次电压

    double  testCur;                        // 最大测试电流
    double  testVol;                        // 最大测试电压

    double  currentTemp;                    // 参考温度
    double  destTemp;                       // 目标温度
};

struct S_PT_Param
{
    S_PT_Base_Param     baseParam;
};

typedef struct CTPT::ctpt_result S_CTPT_Result;

#endif // PARAMDEF

