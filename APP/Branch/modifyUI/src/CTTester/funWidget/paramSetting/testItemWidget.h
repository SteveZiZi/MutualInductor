#ifndef CTestItemWidget_H
#define CTestItemWidget_H

#include <QGroupBox>

class CCtTester;
class MQCheckBox;
class CTestItemWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit CTestItemWidget(QWidget *parent = 0);

signals:

public slots:
    void inductorChang(int type);

protected:
    void focusInEvent(QFocusEvent * event);
    bool eventFilter(QObject *obj, QEvent *eve);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void onCheckBoxClicked(int status);                            // 设置CtTester中对应的数据

private:
    void initWidget();
    void onCurrentType();
    void onVoltageType();
    void setDefaultValue();
    void addEventFilter();

private:
    MQCheckBox          *m_pExcitationCheck;                        // 励磁
    MQCheckBox          *m_pLoadCheck;                              // 负荷
    MQCheckBox          *m_pChangeRateCheck;                        // 变比
    MQCheckBox          *m_pResistanceCheck;                        // 电阻
};

#endif // CTestItemWidget_H
