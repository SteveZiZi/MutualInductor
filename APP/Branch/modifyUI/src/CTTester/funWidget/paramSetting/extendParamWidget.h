#ifndef CEXTENDPARAMWIDGET_H
#define CEXTENDPARAMWIDGET_H

#include <QGroupBox>

class QLineEdit;
class MQComboBox;
class QLabel;
class CDataManager;

class CExtendParamWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit CExtendParamWidget(QWidget *parent = 0);

signals:

public slots:
    void inductorChang(int type);

    void onChangeChecked(bool status);                                      // 变比
    void onLevelChanged(int index);

protected:
    void focusInEvent(QFocusEvent * event);
    bool eventFilter(QObject *obj, QEvent *eve);
//    void keyPressEvent(QKeyEvent *event);

private slots:
    void onDutyCycChanged(int index);                                       // 循环方式
    void onEditingFinished(const QString &);                                // QMLineEdit控件

private:
    void initWidget();
    void onCurrentType();
    void onVoltageType();
    void setControlMinHeight();
    void setGroupLayout();
    void localConnect();
    void setAllControlHide();
    void addEventFilter();

    void onLevelPShow();                                                    // p级 显示
    void onLevelTpyShow();                                                  // tpy级  显示
    void onLevelMeasureShow();                                              // 计量级 显示
    void onLevelPrShow();                                                   // pr级
    void onLevelPxShow();                                                   // px级
    void onLevelTpsShow();                                                  // tps级显示
    void onLevelTpxShow();                                                  // tpx级显示
    void onLevelTpzShow();                                                  // tpz级显示
    void onChangeCheckedShow();                                             // 变比显示

    void setPLevelData();                                                   // p级
    void setTpyLevelData();                                                 // tpy级
    void setMeasureLevelData();                                             // 计量级
    void setPrLevelData();                                                  // pr级
    void setPxLevelData();                                                  // px级
    void setTpsLevelData();                                                 // tps级显示
    void setTpxLevelData();                                                 // tpx级显示
    void setTpzLevelData();                                                 // tpz级显示
    void setPtExtendData();                                                 // pt类型扩展参数
    void setLineEditsReg();                                                 // 设置输入的正则表达式

private:
    CDataManager       *m_dataMgr;

    QLineEdit          *m_pAlf;                                     // 额定准确限值系数
    QLineEdit          *m_pKssc;                                    // 额定对称短路电流系数
    QLineEdit          *m_pKtd;                                     // 暂态面积系数
    QLineEdit          *m_pTp;                                      // 一次时间常数
    QLineEdit          *m_pTs;                                      // 二次时间常数
    MQComboBox         *m_pDutyCycle;                               // 工作循环
    QLineEdit          *m_pT1;                                      // t1
    QLineEdit          *m_pTal1;                                    // tal1
    QLineEdit          *m_pTfr;                                     // tfr
    QLineEdit          *m_pT2;                                      // t2
    QLineEdit          *m_pTal2;                                    // tal2
    QLineEdit          *m_pFS;                                      // 额定仪表保安系数
    QLineEdit          *m_pExtendCurrent;                           // 扩展电流标定
    QLineEdit          *m_pKx;                                      // 额定计算系数Kx
    QLineEdit          *m_pEk;                                      // 额定拐点电势
    QLineEdit          *m_pIe;                                      // Ek对应Ie
    QLineEdit          *m_pUal;                                     // 额定ual
    QLineEdit          *m_pIal;                                     // Ual对应最大Ial

    QLabel              *m_pAlfL;                                   // 额定准确限值系数
    QLabel              *m_pKsscL;                                  // 额定对称短路电流系数
    QLabel              *m_pKtdL;                                   // 暂态面积系数
    QLabel              *m_pTpL;                                    // 一次时间常数
    QLabel              *m_pTsL;                                    // 二次时间常数
    QLabel              *m_pDutyCycleL;                             // 工作循环
    QLabel              *m_pT1L;                                    // t1
    QLabel              *m_pTal1L;                                  // tal1
    QLabel              *m_pTfrL;                                   // tfr
    QLabel              *m_pT2L;                                    // t2
    QLabel              *m_pTal2L;                                  // tal2
    QLabel              *m_pFSL;                                    // 额定仪表保安系数
    QLabel              *m_pExtendCurrentL;                         // 扩展电流标定
    QLabel              *m_pKxL;                                    // 额定计算系数Kx
    QLabel              *m_pEkL;                                    // 额定拐点电势
    QLabel              *m_pIeL;                                    // Ek对应Ie
    QLabel              *m_pUalL;                                   // 额定Ual
    QLabel              *m_pIalL;                                   // Ual对应最大Ial
};

#endif // CEXTENDPARAMWIDGET_H
