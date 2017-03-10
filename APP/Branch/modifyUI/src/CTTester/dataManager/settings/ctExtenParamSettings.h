#ifndef CPTEXTENPARAMSETTINGS_H
#define CPTEXTENPARAMSETTINGS_H

#include "src/CTTester/dataManager/device/paramdef.h"

class QDomDocument;
class QDomElement;

class CCTExtendParamSettings_Measure
{
public:
    CCTExtendParamSettings_Measure();
    virtual ~CCTExtendParamSettings_Measure();

    const S_CT_Measure_Extend_Param &param() const {return m_sSettings;}

    // 额定仪表保安系数
    double fs() const {return m_sSettings.fs;}
    void setFs(const double fs_) {m_sSettings.fs = fs_;}

    // 扩展电流标定
    double extendCurrent() const {return m_sSettings.extendCurrent;}
    void setExtendCurrent(const double extendCurrent_) {m_sSettings.extendCurrent = extendCurrent_;}

    // 配置加载/保存
    bool loadParam(QDomElement* pCTSetELem);
    bool saveParam(QDomDocument* pDoc, QDomElement* pCTSetELem);

private:
    void defaultSettings();

private:
    S_CT_Measure_Extend_Param             m_sSettings;
};

class CCTExtendParamSettings_P
{
public:
    CCTExtendParamSettings_P();
    virtual ~CCTExtendParamSettings_P();

    const S_CT_P_Extend_Param &param() const {return m_sSettings;}

    // 额定准确限值系数
    double alf() const {return m_sSettings.alf;}
    void setAlf(double alf_) {m_sSettings.alf = alf_;}

    // 配置加载/保存
    bool loadParam(QDomElement* pCTSetELem);
    bool saveParam(QDomDocument* pDoc, QDomElement* pCTSetELem);

private:
    void defaultSettings();

private:
    S_CT_P_Extend_Param             m_sSettings;
};

class CCTExtendParamSettings_PR
{
public:
    CCTExtendParamSettings_PR();
    virtual ~CCTExtendParamSettings_PR();

    const S_CT_PR_Extend_Param &param() const {return m_sSettings;}

    // 额定准确限值系数
    double alf() const {return m_sSettings.alf;}
    void setAlf(const double alf_) {m_sSettings.alf = alf_;}

    // 二次时间常数
    double Ts() const {return m_sSettings.Ts;}
    void setTs(const double Ts_) {m_sSettings.Ts = Ts_;}

    // 配置加载/保存
    bool loadParam(QDomElement* pCTSetELem);
    bool saveParam(QDomDocument* pDoc, QDomElement* pCTSetELem);

private:
    void defaultSettings();

private:
    S_CT_PR_Extend_Param             m_sSettings;
};

class CCTExtendParamSettings_PX
{
public:
    CCTExtendParamSettings_PX();
    virtual ~CCTExtendParamSettings_PX();

    const S_CT_PX_Extend_Param &param() const {return m_sSettings;}

    // 额定计算系数Kx
    double Kx() const {return m_sSettings.Kx;}
    void setKx(const double kx_) {m_sSettings.Kx = kx_;}

    // 额定拐点电势
    double Ek() const {return m_sSettings.Ek;}
    void setEk(const double ek_) {m_sSettings.Ek = ek_;}

    // Ek对应Ie
    double Ie() const {return m_sSettings.Ie;}
    void setIe(const double ie_) {m_sSettings.Ie = ie_;}

    // 配置加载/保存
    bool loadParam(QDomElement* pCTSetELem);
    bool saveParam(QDomDocument* pDoc, QDomElement* pCTSetELem);

private:
    void defaultSettings();

private:
    S_CT_PX_Extend_Param             m_sSettings;
};

class CCTExtendParamSettings_TPX
{
public:
    CCTExtendParamSettings_TPX();
    virtual ~CCTExtendParamSettings_TPX();

    const S_CT_TPX_Extend_Param &param() const {return m_sSettings;}

    // 额定对称短路电流系数
    double kssc() const {return m_sSettings.kssc;}
    void setKssc(const double kssc_) {m_sSettings.kssc = kssc_;}

    // 暂态面积系数
    double ktd() const {return m_sSettings.ktd;}
    void setKtd(const double ktd_) {m_sSettings.ktd = ktd_;}

    // 一次时间常数
    double Tp() const {return m_sSettings.Tp;}
    void setTp(const double tp_) {m_sSettings.Tp = tp_;}

    // 二次时间常数
    double Ts() const {return m_sSettings.Ts;}
    void setTs(const double ts_) {m_sSettings.Ts = ts_;}

    // t1
    double t1() const {return m_sSettings.t1;}
    void setT1(const double t1_) {m_sSettings.t1 = t1_;}

    // tal1
    double tal1() const {return m_sSettings.tal1;}
    void setTal1(const double tal1_) {m_sSettings.tal1 = tal1_;}

    // 工作循环
    int coc() const {return m_sSettings.coc;}
    void setCoc(const int coc_) {m_sSettings.coc = coc_;}

    // tfr
    double tfr() const {return m_sSettings.tfr;}
    void setTfr(const double tfr_) {m_sSettings.tfr = tfr_;}

    // t2
    double t2() const {return m_sSettings.t2;}
    void setT2(const double t2_) {m_sSettings.t2 = t2_;}

    // tal2
    double tal2() const {return m_sSettings.tal2;}
    void setTal2(const double tal2_) {m_sSettings.tal2 = tal2_;}

    // 配置加载/保存
    bool loadParam(QDomElement* pCTSetELem);
    bool saveParam(QDomDocument* pDoc, QDomElement* pCTSetELem);

private:
    void defaultSettings();

private:
    S_CT_TPX_Extend_Param             m_sSettings;
};

class CCTExtendParamSettings_TPY
{
public:
    CCTExtendParamSettings_TPY();
    virtual ~CCTExtendParamSettings_TPY();

    const S_CT_TPY_Extend_Param &param() const {return m_sSettings;}

    // 额定对称短路电流系数
    double kssc() const {return m_sSettings.kssc;}
    void setKssc(const double kssc_) {m_sSettings.kssc = kssc_;}

    // 暂态面积系数
    double ktd() const {return m_sSettings.ktd;}
    void setKtd(const double ktd_) {m_sSettings.ktd = ktd_;}

    // 一次时间常数
    double Tp() const {return m_sSettings.Tp;}
    void setTp(const double tp_) {m_sSettings.Tp = tp_;}

    // 二次时间常数
    double Ts() const {return m_sSettings.Ts;}
    void setTs(const double ts_) {m_sSettings.Ts = ts_;}

    // t1
    double t1() const {return m_sSettings.t1;}
    void setT1(const double t1_) {m_sSettings.t1 = t1_;}

    // tal1
    double tal1() const {return m_sSettings.tal1;}
    void setTal1(const double tal1_) {m_sSettings.tal1 = tal1_;}

    // 工作循环
    int coc() const {return m_sSettings.coc;}
    void setCoc(const int coc_) {m_sSettings.coc = coc_;}

    // tfr
    double tfr() const {return m_sSettings.tfr;}
    void setTfr(const double tfr_) {m_sSettings.tfr = tfr_;}

    // t2
    double t2() const {return m_sSettings.t2;}
    void setT2(const double t2_) {m_sSettings.t2 = t2_;}

    // tal2
    double tal2() const {return m_sSettings.tal2;}
    void setTal2(const double tal2_) {m_sSettings.tal2 = tal2_;}

    // 配置加载/保存
    bool loadParam(QDomElement* pCTSetELem);
    bool saveParam(QDomDocument* pDoc, QDomElement* pCTSetELem);

private:
    void defaultSettings();

private:
    S_CT_TPY_Extend_Param             m_sSettings;
};

class CCTExtendParamSettings_TPZ
{
public:
    CCTExtendParamSettings_TPZ();
    virtual ~CCTExtendParamSettings_TPZ();

    const S_CT_TPZ_Extend_Param &param() const {return m_sSettings;}

    // 额定对称短路电流系数
    double kssc() const {return m_sSettings.kssc;}
    void setKssc(const double kssc_) {m_sSettings.kssc = kssc_;}

    // 暂态面积系数
    double ktd() const {return m_sSettings.ktd;}
    void setKtd(const double ktd_) {m_sSettings.ktd = ktd_;}

    // 一次时间常数
    double Tp() const {return m_sSettings.Tp;}
    void setTp(const double tp_) {m_sSettings.Tp = tp_;}

    // 二次时间常数
    double Ts() const {return m_sSettings.Ts;}
    void setTs(const double ts_) {m_sSettings.Ts = ts_;}

    // 配置加载/保存
    bool loadParam(QDomElement* pCTSetELem);
    bool saveParam(QDomDocument* pDoc, QDomElement* pCTSetELem);

private:
    void defaultSettings();

private:
    S_CT_TPZ_Extend_Param             m_sSettings;
};

class CCTExtendParamSettings_TPS
{
public:
    CCTExtendParamSettings_TPS();
    virtual ~CCTExtendParamSettings_TPS();

    const S_CT_TPS_Extend_Param &param() const {return m_sSettings;}

    // 额定对称短路电流系数
    double kssc() const {return m_sSettings.kssc;}
    void setKssc(const double kssc_) {m_sSettings.kssc = kssc_;}

    // 暂态面积系数
    double ktd() const {return m_sSettings.ktd;}
    void setKtd(const double ktd_) {m_sSettings.ktd = ktd_;}

    // 额定ual
    double Ual() const {return m_sSettings.Ual;}
    void setUal(const double ual_) {m_sSettings.Ual = ual_;}

    // Ual对应最大Ia1
    double Ial() const {return m_sSettings.Ial;}
    void setIal(const double Ial_) {m_sSettings.Ial = Ial_;}

    // 配置加载/保存
    bool loadParam(QDomElement* pCTSetELem);
    bool saveParam(QDomDocument* pDoc, QDomElement* pCTSetELem);

private:
    void defaultSettings();

private:
    S_CT_TPS_Extend_Param             m_sSettings;
};


#endif // CPTEXTENPARAMSETTINGS_H
