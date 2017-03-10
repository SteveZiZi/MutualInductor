#include "waveshowwidget.h"
#include "ui_waveshowwidget.h"
#include "./diagramCtrl/BackGround.h"
#include "./diagramCtrl/Foreground.h"
#include "src/fpga/raw_wave.hpp"

waveShowWidget::waveShowWidget(QWidget *parent) :
    QWidget(parent),
    isCurrentWave(false)
{
}

waveShowWidget::~waveShowWidget()
{
}

void waveShowWidget::createTrend(bool isCurrentWave)
{
    this->isCurrentWave = isCurrentWave;

    QRect rect = this->rect();
    m_pCtrl = new MDIAGRAM::CDiagramCtrl(NULL, NULL, this);
    m_pCtrl->setGeometry(rect);

    m_tScaleInfo.fScaleX = 1.00;
    m_tScaleInfo.gridCountX = MAX_POINT_COUNT;
    m_tScaleInfo.intervalX = MAX_POINT_COUNT/4;
    m_tScaleInfo.divIntervalX = MAX_POINT_COUNT/16;
    m_tScaleInfo.fScaleY = 0.1;
    m_tScaleInfo.gridCountY = 200.0;
    m_tScaleInfo.intervalY = 20;
    m_tScaleInfo.divIntervalY = m_tScaleInfo.intervalY;

    m_tScaleInfo.magin.setMargin(50, 20, 10, 20);
    m_tScaleInfo.rectClient = m_pCtrl->rect();
    m_tScaleInfo.axisDirY = MDIAGRAM::POSITIVE|MDIAGRAM::NEGATIVE;
    m_tScaleInfo.axisDirX = MDIAGRAM::POSITIVE;

    MDIAGRAM::CBackGround* pBackGround = new MDIAGRAM::CBackGround(MDIAGRAM::BACKGROUND_ID);
    m_pScale = new MDIAGRAM::CScale(MDIAGRAM::SCALE_ID, pBackGround);
    m_pScale->SetShowTitle(true, true);
    if (isCurrentWave)
        m_pScale->SetTitle("N", "A");
    else
        m_pScale->SetTitle("N", "V");
    m_pScale->AttachScale(&m_tScaleInfo);
    m_pScale->setScaleValPos(40, -1);
    m_pScale->SetTitleColor(qRgb(255,255,255));
    m_pScale->SetScaleValColor(qRgb(255,255,255));

    MDIAGRAM::CForeground* pForeground = new MDIAGRAM::CForeground(MDIAGRAM::FOREGROUND_ID);
    m_pTrend = new DateTrend(MDIAGRAM::TREND_ID, pForeground);
    m_pTrend->AttachScale(&m_tScaleInfo);
    m_pTrend->SetColor(0, qRgb(255,0,255));

    m_pCtrl->SetComponent(m_pScale, m_pTrend);
    m_pCtrl->Init();
}

void waveShowWidget::changeWave(bool isCurrentWave)
{
    this->isCurrentWave = isCurrentWave;

    if (isCurrentWave)
        m_pScale->SetTitle("N", "A");
    else
        m_pScale->SetTitle("N", "V");

    m_pCtrl->Init();
    m_pCtrl->repaint();
}
#define FACTOR 5.0
void waveShowWidget::updateTrend(raw_wave * wave)
{
    const std::vector<double> &waveData = wave->get_readonly_buffer();
    DateTrend::WaveData data;
    unsigned int np = waveData.size();
    unsigned int step = 1;
    if (np > MAX_POINT_COUNT) {
        step = np/MAX_POINT_COUNT;
        np = MAX_POINT_COUNT;
    }

    for(unsigned int i = 0; i<np; i++)
    {
        data.xData = i;
        data.yData[0] = waveData[i*step];
        if (m_pTrend->DataCount()<i)
            m_pTrend->AppendData(data);
        else
            m_pTrend->ReplaceData(i, data);
    }

    wave->calculate_peak();
    double peak = wave->get_peek();
    double range = m_tScaleInfo.fScaleY*m_tScaleInfo.gridCountY/2;
    bool bErase = false;
    do {
        if (peak>range) {
            m_tScaleInfo.fScaleY *= FACTOR;
            bErase = true;
        }
        else if (peak < range/(2*FACTOR)) {
            m_tScaleInfo.fScaleY /= FACTOR;
            bErase = true;
        }
        else
            break;
        range = m_tScaleInfo.fScaleY*m_tScaleInfo.gridCountY/2;
    }while(1);
    if (bErase) {
        m_pCtrl->DrawBkgnd();
        m_pCtrl->ResetForgnd();
    }

    m_pCtrl->EraseWave();
    m_pCtrl->DrawForegound();
    m_pCtrl->repaint();
}
