#ifndef _BASE_PAGE_WIDGET_H_H_H
#define _BASE_PAGE_WIDGET_H_H_H

#include <QWidget>

class CBasePageWidget : public QWidget
{
    Q_OBJECT
public:
    CBasePageWidget( QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~CBasePageWidget();

protected:
    virtual void showEvent(QShowEvent *);
};

#endif
