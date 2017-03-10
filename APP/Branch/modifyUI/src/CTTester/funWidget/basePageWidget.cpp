#include "basePageWidget.h"

#include "src/CTTester/menuBar/menuBar.h"


CBasePageWidget::CBasePageWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{

}

CBasePageWidget::~CBasePageWidget()
{

}

void CBasePageWidget::showEvent(QShowEvent *event)
{
    CMenuBar* menuBar = GetMenuBar();
    menuBar->updateActions(this->actions());

    QWidget::showEvent(event);
}
