#ifndef CBASEPARAMSETTINGSWIDGET_H
#define CBASEPARAMSETTINGSWIDGET_H

#include <QWidget>

class QLineEdit;
class QLabel;
class MQComboBox;
class QGroupBox;

class CBaseParamSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CBaseParamSettingsWidget(QWidget *parent = 0);

signals:
    //  互感器级别发生变化是发出该信号
    void inductorClassChanged(int ctpt_class);

public slots:
    void inductorChang(int type);

private slots:
    //  编辑框完成编辑后的处理
    void finishEdit(const QString &);
    //  互感器等级发生变化后的处理
    void onLevelChanged(int);

protected:
    void focusInEvent(QFocusEvent *event);
    bool eventFilter(QObject *obj, QEvent *eve);
    void keyPressEvent(QKeyEvent *event);

private:
    void initWidget();

    void onCurrentType();
    void onVoltageType();
    void createInductorName();
    void createBaseParam();
    void createRatioParam();
    void createResParam();
    void setDefaultValue();
    void setLineEditsReg();                                         // 设置输入的正则表达式
    void addEventFilter();

private:
    QGroupBox* groupBox_name;
    QLineEdit* m_serialNumEdit;
    QLineEdit* m_coilNumEdit;

    QGroupBox* groupBox_baseParam;
    MQComboBox* m_coilLevelCombo;
    QLineEdit* m_testCurEdit;

    QGroupBox* groupBox_ratio;
    QLabel   * m_primaryValLabel;
    QLineEdit* m_primaryValEdit;
    QLabel   * m_primaryUnitLabel;
    QLabel   * m_secondValLabel;
    QLineEdit* m_secondValEdit;
    QLabel   * m_secondUnitLabel;
    QLineEdit* m_ratedLoadEdit;
    QLineEdit* m_powerFactorEdit;

    QGroupBox* groupBox_res;
    QLineEdit* m_testTempEdit;
    QLineEdit* m_destTempEdit;
};

#endif // CBASEPARAMSETTINGSWIDGET_H
