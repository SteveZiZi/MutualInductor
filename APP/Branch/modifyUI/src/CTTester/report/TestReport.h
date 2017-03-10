#ifndef _TEST_REPORT_H_H_H
#define _TEST_REPORT_H_H_H

#include "src/CTTester/dataManager/device/paramdef.h"
#include <QDomDocument>

class CTestReport {
public:
    CTestReport();
    ~CTestReport();

    const S_CTPT_Result & result() {return m_result;}
    const S_CT_Param & ctParam() {return m_ctParam;}
    const S_PT_Param & ptParam() {return m_ptParam;}

    bool saveReport(const QString & reportName, const S_CT_Param & param, int testItem, const S_CTPT_Result & result);
    bool saveReport(const QString & reportName, const S_PT_Param & param, int testItem, const S_CTPT_Result & result);

private:
    void saveParam(QDomDocument &doc, QDomElement &root, const S_CT_Param & param, int testItem);
    void saveResult(QDomDocument &doc, QDomElement &root,int testItem,const S_CTPT_Result & result);

private:
    QString m_path;

    S_CT_Param m_ctParam;
    S_PT_Param m_ptParam;
    S_CTPT_Result m_result;
};

#endif
