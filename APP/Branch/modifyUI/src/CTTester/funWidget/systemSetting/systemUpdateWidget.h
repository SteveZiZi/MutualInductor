#ifndef CSYSTEMUPDATEWIDGET_H
#define CSYSTEMUPDATEWIDGET_H

#include <QWidget>

class QGroupBox;
class CSystemUpdateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CSystemUpdateWidget(QWidget *parent = 0);

signals:

public slots:

protected:
    void focusInEvent(QFocusEvent * event);

private:
    void initWidget();
    void setMainLayout();
    void localConnect();

private:
    QGroupBox                       *m_pUpdateGb;
};

#endif // CSYSTEMUPDATEWIDGET_H
