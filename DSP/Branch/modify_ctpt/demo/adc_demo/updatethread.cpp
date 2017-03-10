#include "updatethread.h"


UpdateThread::UpdateThread(settingsWidget *waveWidget, QObject *parent)
    :QThread(parent)
    ,m_waveWidget(waveWidget)
{

}

void UpdateThread::run()
{
    while (this->isRunning()) {
        if (m_waveWidget->getWave())
            emit updateShow();
    }
}
