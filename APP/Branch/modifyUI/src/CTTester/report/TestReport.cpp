#include "TestReport.h"

#include <QFile>

#define NODE_TYPE  "type"
#define NODE_LEVEL "level"
#define NODE_RES   "resistance"


static inline QString type_to_str(const CTPT::ctpt_type type)
{
    return (type == CTPT::TYPE_CT)?"CT":"PT";
}
static inline CTPT::ctpt_type str_to_type(const QString & type)
{
    return (type == "CT") ? CTPT::TYPE_CT : CTPT::TYPE_PT;
}

static inline QString level_to_str(const CTPT_CLASS_ENUM level)
{
    switch(level) {
    case CTPT_P:
        return "P";
    case CTPT_PR:
        return "PR";
    case CTPT_PX:
        return "PX";
    case CTPT_MEASURE:
        return "Measure";
    case CTPT_TPX:
        return "TPX";
    case CTPT_TPY:
        return "TPY";
    case CTPT_TPZ:
        return "TPZ";
    case CTPT_TPS:
        return "TPS";
    default:
        return "P";
    }
}
static inline CTPT_CLASS_ENUM str_to_level(const QString & level)
{
    if (level == "P") return CTPT_P;
    else if (level == "PR") return CTPT_PR;
    else if (level == "PX") return CTPT_PX;
    else if (level == "Measure") return CTPT_MEASURE;
    else if (level == "TPX") return CTPT_TPX;
    else if (level == "TPY") return CTPT_TPY;
    else if (level == "TPZ") return CTPT_TPZ;
    else if (level == "TPS") return CTPT_TPS;
    else return CTPT_P;
}

#define ADD_ELE(root, eleName, eleVal) \
    do { \
        QDomElement ele = doc.createElement(eleName); \
        ele.setAttribute("val", eleVal); \
        root.appendChild(ele); \
    } while(0)


CTestReport::CTestReport()
    : m_path("./")
{

}

CTestReport::~CTestReport()
{

}

bool CTestReport::saveReport(const QString &reportName,
                             const S_CT_Param &param,
                             int testItem,
                             const S_CTPT_Result &result)
{
    QDomDocument doc;
    doc.appendChild(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));
    QDomElement root = doc.createElement("reports");
    doc.appendChild(root);

    saveParam(doc, root, param, testItem);
    saveResult(doc, root, testItem, result);


    QString fileName = m_path + reportName;
    QFile file(fileName);
    if (!file.open(QFile::ReadWrite | QFile::Truncate | QFile::Text))
        return false;
    QByteArray text(doc.toByteArray());
    file.write(text.data(), text.size());
    file.flush();
    file.close();
}

bool CTestReport::saveReport(const QString &reportName,
                             const S_PT_Param &param,
                             int testItem,
                             const S_CTPT_Result &result)
{

    return true;
}


void CTestReport::saveParam(QDomDocument &doc, QDomElement &root, const S_CT_Param &param, int testItem)
{
    QDomElement paramEle = doc.createElement("Param");
    paramEle.setAttribute("type", "CT");
    root.appendChild(paramEle);

    ADD_ELE(paramEle, "testItem", QString::number(testItem, 16));

    //baseParam
    QDomElement baseParam = doc.createElement("baseParam");
    paramEle.appendChild(baseParam);
    ADD_ELE(baseParam, "serialNumber", param.baseParam.serialNumber);
    ADD_ELE(baseParam, "winding", param.baseParam.winding);
    ADD_ELE(baseParam, "level", level_to_str(param.baseParam.level));
    ADD_ELE(baseParam, "testCurrent", QString::number(param.baseParam.testCurrent));
    ADD_ELE(baseParam, "primaryCurrent", QString::number(param.baseParam.primaryCurrent));
    ADD_ELE(baseParam, "secondCurrent", QString::number(param.baseParam.secondCurrent));
    ADD_ELE(baseParam, "ratedLoad", QString::number(param.baseParam.ratedLoad));
    ADD_ELE(baseParam, "powerFactor", QString::number(param.baseParam.powerFactor));
    ADD_ELE(baseParam, "currentTemp", QString::number(param.baseParam.currentTemp));
    ADD_ELE(baseParam, "destTemp", QString::number(param.baseParam.destTemp));

    //extendParam
    QDomElement extendParam = doc.createElement("extendParam");
    paramEle.appendChild(extendParam);

    switch (param.baseParam.level) {
    case CTPT_P:
        ADD_ELE(extendParam, "ALF", QString::number(param.extend.pLevelParam.alf));
        break;
    case CTPT_MEASURE:
        ADD_ELE(extendParam, "FS", QString::number(param.extend.measureLevelParam.fs));
        ADD_ELE(extendParam, "extendCurrent", QString::number(param.extend.measureLevelParam.extendCurrent));
        break;
    case CTPT_PR:
        ADD_ELE(extendParam, "ALF", QString::number(param.extend.prLevelParam.alf));
        ADD_ELE(extendParam, "Ts", QString::number(param.extend.prLevelParam.Ts));
        break;
    case CTPT_PX:
        ADD_ELE(extendParam, "Kx", QString::number(param.extend.pxLevelParam.Kx));
        ADD_ELE(extendParam, "Ek", QString::number(param.extend.pxLevelParam.Ek));
        ADD_ELE(extendParam, "Ie", QString::number(param.extend.pxLevelParam.Ie));
        break;
    case CTPT_TPX:
        ADD_ELE(extendParam, "KSSC", QString::number(param.extend.tpxLevelParam.kssc));
        ADD_ELE(extendParam, "KTD", QString::number(param.extend.tpxLevelParam.ktd));
        ADD_ELE(extendParam, "Tp", QString::number(param.extend.tpxLevelParam.Tp));
        ADD_ELE(extendParam, "Ts", QString::number(param.extend.tpxLevelParam.Ts));
        ADD_ELE(extendParam, "t1", QString::number(param.extend.tpxLevelParam.t1));
        ADD_ELE(extendParam, "tal1", QString::number(param.extend.tpxLevelParam.tal1));
        ADD_ELE(extendParam, "coc", QString::number(param.extend.tpxLevelParam.coc));
        ADD_ELE(extendParam, "tfr", QString::number(param.extend.tpxLevelParam.tfr));
        ADD_ELE(extendParam, "t2", QString::number(param.extend.tpxLevelParam.t2));
        ADD_ELE(extendParam, "tal2", QString::number(param.extend.tpxLevelParam.tal2));
        break;
    case CTPT_TPY:
        ADD_ELE(extendParam, "KSSC", QString::number(param.extend.tpyLevelParam.kssc));
        ADD_ELE(extendParam, "KTD", QString::number(param.extend.tpyLevelParam.ktd));
        ADD_ELE(extendParam, "Tp", QString::number(param.extend.tpyLevelParam.Tp));
        ADD_ELE(extendParam, "Ts", QString::number(param.extend.tpyLevelParam.Ts));
        ADD_ELE(extendParam, "t1", QString::number(param.extend.tpyLevelParam.t1));
        ADD_ELE(extendParam, "tal1", QString::number(param.extend.tpyLevelParam.tal1));
        ADD_ELE(extendParam, "coc", QString::number(param.extend.tpyLevelParam.coc));
        ADD_ELE(extendParam, "tfr", QString::number(param.extend.tpyLevelParam.tfr));
        ADD_ELE(extendParam, "t2", QString::number(param.extend.tpyLevelParam.t2));
        ADD_ELE(extendParam, "tal2", QString::number(param.extend.tpyLevelParam.tal2));
        break;
    case CTPT_TPZ:
        ADD_ELE(extendParam, "KSSC", QString::number(param.extend.tpzLevelParam.kssc));
        ADD_ELE(extendParam, "KTD", QString::number(param.extend.tpzLevelParam.ktd));
        ADD_ELE(extendParam, "Tp", QString::number(param.extend.tpzLevelParam.Tp));
        ADD_ELE(extendParam, "Ts", QString::number(param.extend.tpzLevelParam.Ts));
        break;
    case CTPT_TPS:
        ADD_ELE(extendParam, "KSSC", QString::number(param.extend.tpsLevelParam.kssc));
        ADD_ELE(extendParam, "KTD", QString::number(param.extend.tpsLevelParam.ktd));
        ADD_ELE(extendParam, "Ual", QString::number(param.extend.tpsLevelParam.Ual));
        ADD_ELE(extendParam, "Ial", QString::number(param.extend.tpsLevelParam.Ial));
        break;
    }
}

void CTestReport::saveResult(QDomDocument &doc, QDomElement &root, int testItem, const S_CTPT_Result &result)
{
    if (testItem == 0)
        return ;

    QDomElement resultEle = doc.createElement("Result");
    resultEle.setAttribute("testItem", QString::number(testItem, 16));
    root.appendChild(resultEle);

    ADD_ELE(resultEle, "type", type_to_str(result.type));
    ADD_ELE(resultEle, "level", level_to_str(static_cast<CTPT_CLASS_ENUM>(result.level)));

    if (testItem & TEST_RESISTANCE) {
        QDomElement resEle = doc.createElement("Resistance");
        resEle.setAttribute("measRes", QString::number(result.widingRes.measRes));
        resEle.setAttribute("refRes", QString::number(result.widingRes.refRes));
        resultEle.appendChild(resEle);
    }
    if (testItem & TEST_BURDEN) {
        QDomElement ele = doc.createElement("Burden");
        ele.setAttribute("powerFactor", QString::number(result.burden.powerFactor));
        ele.setAttribute("Z", QString::number(result.burden.Z));
        ele.setAttribute("measBurden", QString::number(result.burden.measBurden));
        resultEle.appendChild(ele);
    }
    if (testItem & TEST_EXCITATION) {
        QDomElement excitationEle = doc.createElement("excitation");
        resultEle.appendChild(excitationEle);

        QDomElement excitResultEle = doc.createElement("Result");
        excitationEle.appendChild(excitResultEle);
        ADD_ELE(excitResultEle, "Vkn", QString::number(result.excitation.Vkn));
        ADD_ELE(excitResultEle, "Ikn", QString::number(result.excitation.Ikn));
        ADD_ELE(excitResultEle, "Ts", QString::number(result.excitation.Ts));
        ADD_ELE(excitResultEle, "Kr", QString::number(result.excitation.Kr));
        switch(result.level) {
        case CTPT::CTC_P:
            ADD_ELE(excitResultEle, "Eal", QString::number(result.excitation.p.Eal));
            ADD_ELE(excitResultEle, "composite_error", QString::number(result.excitation.p.composite_error));
            ADD_ELE(excitResultEle, "ALF", QString::number(result.excitation.p.alf));
            break;
        case CTPT::CTC_MEASURE:
            ADD_ELE(excitResultEle, "Eal", QString::number(result.excitation.measure.Eal));
            ADD_ELE(excitResultEle, "composite_error", QString::number(result.excitation.measure.composite_error));
            ADD_ELE(excitResultEle, "FS", QString::number(result.excitation.measure.fs));
            break;
        case CTPT::CTC_PX:
            ADD_ELE(excitResultEle, "Kx", QString::number(result.excitation.px.Kx));
            ADD_ELE(excitResultEle, "Ek", QString::number(result.excitation.px.Ek));
            ADD_ELE(excitResultEle, "Ie", QString::number(result.excitation.px.Ie));
            ADD_ELE(excitResultEle, "composite_error", QString::number(result.excitation.px.composite_error));
            break;
        case CTPT::CTC_PR:
            ADD_ELE(excitResultEle, "Eal", QString::number(result.excitation.pr.Eal));
            ADD_ELE(excitResultEle, "composite_error", QString::number(result.excitation.pr.composite_error));
            ADD_ELE(excitResultEle, "ALF", QString::number(result.excitation.pr.alf));
            break;
        case CTPT::CTC_TPX:
        case CTPT::CTC_TPY:
        case CTPT::CTC_TPZ:
            ADD_ELE(excitResultEle, "Eal", QString::number(result.excitation.tpx.Eal));
            ADD_ELE(excitResultEle, "peek_error", QString::number(result.excitation.tpx.peek_error));
            ADD_ELE(excitResultEle, "kssc", QString::number(result.excitation.tpx.kssc));
            ADD_ELE(excitResultEle, "ktd", QString::number(result.excitation.tpx.ktd));
            break;
        default:
            break;
        }

        QDomElement excitTrendEle = doc.createElement("Trend");
        excitationEle.appendChild(excitTrendEle);
        Q_FOREACH(const CTPT::ui_wave_data & data, result.exciting_data) {
            QDomElement ele = doc.createElement("data");
            ele.setAttribute("vol", QString::number(data.vol));
            ele.setAttribute("cur", QString::number(data.cur));
            excitTrendEle.appendChild(ele);
        }

        QDomElement error5Ele = doc.createElement("error5");
        excitationEle.appendChild(error5Ele);
        Q_FOREACH(const CTPT::error_data & data, result.error5_data) {
            QDomElement ele = doc.createElement("data");
            ele.setAttribute("Zload", QString::number(data.Zload));
            ele.setAttribute("Kalf", QString::number(data.Kalf));
            error5Ele.appendChild(ele);
        }

        QDomElement error10Ele = doc.createElement("error10");
        excitationEle.appendChild(error10Ele);
        Q_FOREACH(const CTPT::error_data & data, result.error10_data) {
            QDomElement ele = doc.createElement("data");
            ele.setAttribute("Zload", QString::number(data.Zload));
            ele.setAttribute("Kalf", QString::number(data.Kalf));
            error10Ele.appendChild(ele);
        }
    }

    if (testItem & TEST_RATIO) {
        QDomElement ratioEle = doc.createElement("ratio");
        resultEle.appendChild(ratioEle);

        QDomElement ratioResultEle = doc.createElement("Result");
        ratioEle.appendChild(ratioResultEle);
        ADD_ELE(ratioResultEle, "ratio", QString::number(result.ratio.ratio));
        ADD_ELE(ratioResultEle, "ratio_rated", QString::number(result.ratio.ratio_rated));
        ADD_ELE(ratioResultEle, "ratio_error", QString::number(result.ratio.ratio_error));
        ADD_ELE(ratioResultEle, "phase_error", QString::number(result.ratio.phase_error));
        ADD_ELE(ratioResultEle, "turns_ratio", QString::number(result.ratio.turns_ratio));
        ADD_ELE(ratioResultEle, "turns_ratio_error", QString::number(result.ratio.turns_ratio_error));
        ADD_ELE(ratioResultEle, "phase_polar", result.ratio.phase_polar?"+":"-");

        QDomElement ratioErrsEle = doc.createElement("ratio_error_list");
        ratioEle.appendChild(ratioErrsEle);
        Q_FOREACH(const CTPT::ratio_error_data & data, result.ratio_error_data) {
            QDomElement ele = doc.createElement("data");
            ele.setAttribute("VA", QString::number(data.VA));
            ele.setAttribute("factor", QString::number(data.factor));
            ele.setAttribute("percent1", QString::number(data.percent1));
            ele.setAttribute("percent5", QString::number(data.percent5));
            ele.setAttribute("percent20", QString::number(data.percent20));
            ele.setAttribute("percent100", QString::number(data.percent100));
            ele.setAttribute("percent120", QString::number(data.percent120));
            ratioErrsEle.appendChild(ele);
        }

        QDomElement phaseErrsEle = doc.createElement("phase_error_list");
        ratioEle.appendChild(phaseErrsEle);
        Q_FOREACH(const CTPT::ratio_error_data & data, result.phase_error_data) {
            QDomElement ele = doc.createElement("data");
            ele.setAttribute("VA", QString::number(data.VA));
            ele.setAttribute("factor", QString::number(data.factor));
            ele.setAttribute("percent1", QString::number(data.percent1));
            ele.setAttribute("percent5", QString::number(data.percent5));
            ele.setAttribute("percent20", QString::number(data.percent20));
            ele.setAttribute("percent100", QString::number(data.percent100));
            ele.setAttribute("percent120", QString::number(data.percent120));
            phaseErrsEle.appendChild(ele);
        }
    }
}
