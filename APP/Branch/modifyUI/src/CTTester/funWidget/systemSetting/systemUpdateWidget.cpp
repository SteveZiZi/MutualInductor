#include "systemUpdateWidget.h"

#include <QGroupBox>
#include <QVBoxLayout>

CSystemUpdateWidget::CSystemUpdateWidget(QWidget *parent) : QWidget(parent)
{
    initWidget();
    localConnect();
}

void CSystemUpdateWidget::focusInEvent(QFocusEvent *event)
{
    focusNextChild();

    return QWidget::focusInEvent(event);
}

void CSystemUpdateWidget::initWidget()
{
    m_pUpdateGb = new QGroupBox(tr("系统升级"));

    setMainLayout();
}

void CSystemUpdateWidget::setMainLayout()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_pUpdateGb);

    this->setLayout(mainLayout);
}

void CSystemUpdateWidget::localConnect()
{

}

