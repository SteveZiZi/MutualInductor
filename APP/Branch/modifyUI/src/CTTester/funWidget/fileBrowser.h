#ifndef _FILE_BROWSER_H_H_H
#define _FILE_BROWSER_H_H_H

#include <QWidget>
#include <QFileInfoList>

class QTableWidget;
class CFileBrowser : public QWidget
{
    Q_OBJECT
public:
    CFileBrowser(const QString &path, QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~CFileBrowser();

protected:
    virtual void keyPressEvent(QKeyEvent*);

private slots:
    void reportSelectionChanged();

private:
    void createReport();
    bool _enumerateDir(const QString &path);
    void initMenu();

private:
    QTableWidget  *m_report;

    QFileInfoList m_fileList;
};

#endif
