#ifndef CSYSTEMABOUTWIDGET_H
#define CSYSTEMABOUTWIDGET_H

#include <QWidget>

class QLabel;
class QGroupBox;
class CDataManager;
class CSystemAboutWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CSystemAboutWidget(QWidget *parent = 0);

signals:

public slots:

protected:
    void focusInEvent(QFocusEvent * event);

private:
    void initWidget();
    void setMainLayout();
    void setCtrlsMinSize();

private:
    CDataManager                        *m_dataMgr;

    QLabel                              *m_pSystemVersionL;
    QLabel                              *m_pArithmeticVersionL;
    QLabel                              *m_pApplicationVersionL;
    QLabel                              *m_pFpgaVersionL;
    QLabel                              *m_pHardwareVersionL;
    QGroupBox                           *m_pAboutGb;
};

#endif // CSYSTEMABOUTWIDGET_H
