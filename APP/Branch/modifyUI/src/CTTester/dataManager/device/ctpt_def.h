#ifndef CTPT_DEF
#define CTPT_DEF

#define CPTAPI

#include <vector>

namespace CTPT {

enum user_test_mode {
    USER_TEST_RUN = 0,
    USER_TEST_SELFCHECK  //用户自测
};

enum ctpt_type
{
    TYPE_CT,
    TYPE_PT
};

//  测试类型
enum ctpt_test_item
{
    ITEM_RESISTANCE = 1,
    ITEM_EXCITATION = 0x02,
    ITEM_RATIO = 0x04,
    ITEM_BURDEN = 0x08,

    ITEM_DEMAGNETIOZATION = 0x100,   //  退磁

};

//  互感器类型
enum ctpt_class
{
    CTC_P,
    CTC_MEASURE,                // 量
    CTC_PR,
    CTC_PX,
    CTC_TPX,
    CTC_TPY,
    CTC_TPZ,
    CTC_TPS,
};

//  ct基本参数
struct CT_Base_Param
{
    enum ctpt_class level;                  // 级别
    int     testItem;                       // 测试项目
    double  testCurrent;                    // 最大测试电流
    double  primaryCurrent;                 // 额定一次电流
    double  secondCurrent;                  // 额定二次电流
    double  rated_frequency;                // 额定频率  暂时固定50Hz

    double  ratedLoad;                      // 额定负荷
    double  powerFactor;                    // 功率因数

    double  currentTemp;                    // 参考温度
    double  destTemp;                       // 目标温度

    double restance;                        //直阻，临时测试用
};

// 测量CT扩展参数
struct CT_Measure_Extend_Param
{
    double  fs;                             // 额定仪表保安系数
    double  extendCurrent;                  // 扩展电流标定
};
//  P级扩展参数
struct CT_P_Extend_Param
{
    double  alf;                            // 额定准确限值系数
};
//  PR级扩展参数
struct CT_PR_Extend_Param
{
    double  alf;                            // 额定准确限值系数
    double  Ts;                             // 二次时间常数
};
//  PX级扩展参数
struct CT_PX_Extend_Param
{
    double  Kx;                             // 额定计算系数Kx
    double  Ek;                             // 额定拐点电势
    double  Ie;                             // Ek对应le
};
//  TPX级扩展参数
struct CT_TPX_Extend_Param
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
typedef struct CT_TPX_Extend_Param CT_TPY_Extend_Param;
//  TPZ级扩展参数
struct CT_TPZ_Extend_Param
{
    double  kssc;                           // 额定对称短路电流系数
    double  ktd;                            // 暂态面积系数
    double  Tp;                             // 一次时间常数
    double  Ts;                             // 二次时间常数
};
//  TPS扩展参数
struct CT_TPS_Extend_Param
{
    double  kssc;                           // 额定对称短路电流系数
    double  ktd;                            // 面积系数Kx
    double  Ual;                            // 额定ual
    double  Ial;                            // Ual对应最大Ia1
};


struct ct_setting
{
    CT_Base_Param   baseParam;
    union
    {
        CT_Measure_Extend_Param   measure;
        CT_P_Extend_Param         p;
        CT_PR_Extend_Param        pr;
        CT_PX_Extend_Param        px;
        CT_TPX_Extend_Param       tpx;
        CT_TPY_Extend_Param       tpy;
        CT_TPZ_Extend_Param       tpz;
        CT_TPS_Extend_Param       tps;
    }extendParam;
};

struct pt_setting
{
    enum ctpt_class level;                  // 级别
    int     testItem;                       // 测试项目

    double  testCurrent;                    // 最大测试电流
    double  testVoltage;                    // 最大测试电压

    double  rated_frequency;                // 额定频率, in
    double  primaryVol;                     // 额定一次电压
    double  secondVol;                      // 额定二次电压

    double  currentTemp;                    // 参考温度
    double  destTemp;                       // 目标温度
};


struct widing_resistance_result {
    double measRes;                         // 测量电阻
    double refRes;                          // 参考电阻
};

struct burden_result {
    double powerFactor;                     // 功率因素
    double Z;                               // 负载阻抗
    double measBurden;                      // 实测负荷
};

struct excitation_result {
    double Vkn;                             //拐点电压
    double Ikn;                             //拐点电流
    double Lu;                              //不饱和电感
    double Ts;                              //二次时间常数
    double Kr;                              //剩磁系数

    union {
        struct {
            double Eal;                             //极限电动势
            double composite_error;                 //复合误差
            double fs;                              //仪表保安系数
        }measure;
        struct {
            double Eal;                             //极限电动势
            double composite_error;                 //复合误差
            double alf;                             //准确限值系数
        }p;
        struct {
            double Kx;                              //计算系数
            double Ek;                              //额定拐点电势
            double Ie;                              //Ek对应的Ie
            double composite_error;                 //复合误差
        }px;
        struct {
            double Eal;                             //极限电动势
            double composite_error;                 //复合误差
            double alf;                             //准确限值系数
        }pr;
        struct {
            double Eal;                             //极限电动势
            double peek_error;                      //峰瞬误差
            double kssc;                            //额定对称短路电流倍数
            double ktd;                             //暂态面积系数
        }tpx, tpy, tpz;
        struct {
            double kssc;                            //额定对称短路电流倍数
            double Ual;                             //额定Ual
            double Ial;                             //Ual下测试的Ial
        }tps;
    };
};

struct ratio_result {
    double ratio;                           // 变比
    double ratio_rated;                     // 额定变比
    double ratio_error;                     // 比值差
    double phase_error;                     // 相位差
    double turns_ratio;                     // 匝数比
    double turns_ratio_error;               // 匝数比误差
    bool   phase_polar;                     // 极性
};

struct ui_wave_data {
    double cur;
    double vol;

    ui_wave_data(const ui_wave_data& copyin) {
        this->vol = copyin.vol; this->cur = copyin.cur;
    }
    ui_wave_data(double cur = 0.0, double vol = 0.0) {
        this->vol = vol;
        this->cur = cur;
    }

    ui_wave_data &operator=(const ui_wave_data &rhs) {
        this->vol = rhs.vol; this->cur = rhs.cur;
        return *this;
    }
    int operator==(const ui_wave_data &rhs) {
        if(this->vol != rhs.vol) return 0;
        if(this->cur != rhs.cur) return 0;
        return 1;
    }
};
struct error_data {
    double Zload;
    double Kalf;

    error_data(const error_data& copyin) {
        this->Zload = copyin.Zload; this->Kalf = copyin.Kalf;
    }
    error_data(double Zload = 0.0, double Kalf = 0.0) {
        this->Zload = Zload;
        this->Kalf = Kalf;
    }

    error_data &operator=(const error_data &rhs) {
        this->Zload = rhs.Zload; this->Kalf = rhs.Kalf;
        return *this;
    }
    int operator==(const error_data &rhs) {
        if(this->Zload != rhs.Zload) return 0;
        if(this->Kalf != rhs.Kalf) return 0;
        return 1;
    }
};
struct ratio_error_data {
    double VA;
    double factor;
    double percent1;
    double percent5;
    double percent20;
    double percent100;
    double percent120;

    ratio_error_data(const ratio_error_data& copyin) {
        this->VA         = copyin.VA;
        this->factor     = copyin.factor;
        this->percent1   = copyin.percent1;
        this->percent5   = copyin.percent5;
        this->percent20  = copyin.percent20;
        this->percent100 = copyin.percent100;
        this->percent120 = copyin.percent120;
    }
    ratio_error_data() {
        this->VA         = 0.0;
        this->factor     = 0.0;
        this->percent1   = 0.0;
        this->percent5   = 0.0;
        this->percent20  = 0.0;
        this->percent100 = 0.0;
        this->percent120 = 0.0;
    }

    ratio_error_data &operator=(const ratio_error_data &rhs) {
        this->VA         = rhs.VA;
        this->factor     = rhs.factor;
        this->percent1   = rhs.percent1;
        this->percent5   = rhs.percent5;
        this->percent20  = rhs.percent20;
        this->percent100 = rhs.percent100;
        this->percent120 = rhs.percent120;
        return *this;
    }
    int operator==(const ratio_error_data &rhs) {
        if(this->VA         != rhs.VA) return 0;
        if(this->factor     != rhs.factor) return 0;
        if(this->percent1   != rhs.percent1) return 0;
        if(this->percent5   != rhs.percent5) return 0;
        if(this->percent20  != rhs.percent20) return 0;
        if(this->percent100 != rhs.percent100) return 0;
        if(this->percent120 != rhs.percent120) return 0;
        return 1;
    }
};



struct ctpt_result
{
    ctpt_type  type;                    // 互感器类型
    enum ctpt_class level;              // 互感器级别

    // 直阻
    struct widing_resistance_result widingRes;

    // 负载
    struct burden_result burden;

    // 励磁
    struct excitation_result excitation;

    // 变比
    struct ratio_result ratio;

    // 励磁曲线
    std::vector<ui_wave_data> exciting_data;

    // 5%误差曲线
    std::vector<error_data> error5_data;

    // 10%误差曲线
    std::vector<error_data> error10_data;

    // 比值差
    std::vector<struct ratio_error_data> ratio_error_data;
    // 相位差
    std::vector<struct ratio_error_data> phase_error_data;

    ctpt_result();
    ctpt_result(const ctpt_result& copyin);
    ~ctpt_result();

    ctpt_result &operator=(const ctpt_result &rhs);

    void clear();
};

}

#endif // CTPT_DEF

