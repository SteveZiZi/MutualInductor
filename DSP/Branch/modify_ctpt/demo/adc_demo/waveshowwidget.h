#ifndef WAVESHOWWIDGET_H
#define WAVESHOWWIDGET_H

#include <QWidget>

#include "./diagramCtrl/diagramCommon.h"
#include "./diagramCtrl/diagramCtrl.h"
#include "./diagramCtrl/Scale.h"
#include "./diagramCtrl/TrendCtrl/Trend.h"


#define MAX_POINT_COUNT 256

class raw_wave;

class waveShowWidget : public QWidget
{
    Q_OBJECT
    typedef MDIAGRAM::CTrend<int, double, 1, MAX_POINT_COUNT> DateTrend;

public:
    explicit waveShowWidget(QWidget *parent = 0);
    ~waveShowWidget();

    void createTrend(bool isCurrentWave);
    void changeWave(bool isCurrentWave);
    void updateTrend(raw_wave *wave);

private:
    MDIAGRAM::CDiagramCtrl      *m_pCtrl;
    MDIAGRAM::CScale            *m_pScale;
    MDIAGRAM::ScaleInfo          m_tScaleInfo;
    DateTrend                   *m_pTrend;

    bool isCurrentWave;
};

#endif // WAVESHOWWIDGET_H
