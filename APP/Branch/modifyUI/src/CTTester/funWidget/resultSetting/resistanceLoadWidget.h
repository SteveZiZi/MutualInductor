#ifndef CRESISTANCELOADWIDGET_H
#define CRESISTANCELOADWIDGET_H

#include <QWidget>

class QLineEdit;
class QGroupBox;
class CDataManager;
class CResistanceLoadWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CResistanceLoadWidget(QWidget *parent = 0);

    void clearTestMark();

signals:

public slots:
    void updateWidget();

protected:
    void focusInEvent(QFocusEvent * event);

private:
    void initWidget();
    void setMainLayout();
    void setLineEditEnable(bool enable);

private:
    CDataManager            *m_dataMgr;

    QLineEdit               *m_pResistance_25;                          // 25℃
    QLineEdit               *m_pResistance_75;                          // 75℃
    QLineEdit               *m_pLoadData;                               // 实测负载
    QLineEdit               *m_pPowerFactor;                            // 功率因数
    QLineEdit               *m_pImpedance;                              // 阻抗

    QGroupBox               *m_pResistanceGb;
    QGroupBox               *m_pLoadGb;
};

#endif // CRESISTANCELOADWIDGET_H
