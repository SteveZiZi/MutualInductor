#ifndef UPDATETHREAD_H
#define UPDATETHREAD_H

#include <QThread>
#include "adc_demo.h"

class UpdateThread : public QThread
{
    Q_OBJECT
public:
    UpdateThread(settingsWidget* waveWidget, QObject *parent = 0);

signals:
    void updateShow();

protected:
    virtual void run();

private:
    settingsWidget* m_waveWidget;
};

#endif // UPDATETHREAD_H
