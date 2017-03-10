#ifndef CMUTUALINDUCTORTYPEWIDGET_H
#define CMUTUALINDUCTORTYPEWIDGET_H

#include <QGroupBox>

class MQCheckBox;

class CMutualInductorTypeWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit CMutualInductorTypeWidget(QWidget *parent = 0);
    int getInductorType();

signals:
    void inductorChanged(int type);                             // 0:CT  1:PT

public slots:

protected:
    void focusInEvent(QFocusEvent * event);
    bool eventFilter(QObject *obj, QEvent *eve);

private slots:
    void onCheckBoxClicked(int);

private:
    void initWidget();
    void setDefauleValue();
    void addEventFilter();

private:
    MQCheckBox           *m_pCurrentType;                            // 电流互感器
    MQCheckBox           *m_pVoltageType;                            // 电压互感器
};

#endif // CMUTUALINDUCTORTYPEWIDGET_H
