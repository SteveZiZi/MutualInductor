#ifndef CSYSTEMDEBUGWIDGET_H
#define CSYSTEMDEBUGWIDGET_H

#include <QWidget>

class MPushButton;
class CSystemDebugWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CSystemDebugWidget(QWidget *parent = 0);

signals:

public slots:

protected:
    void focusInEvent(QFocusEvent * event);
    bool eventFilter(QObject *obj, QEvent *eve);

private slots:
    void onBtnClicked();

private:
    void initWidget();
    void setMainLayout();
    void localConnect();
    void addEventFilter();

private:
    MPushButton                        *m_pAdjustBtn;
};

#endif // CSYSTEMDEBUGWIDGET_H
