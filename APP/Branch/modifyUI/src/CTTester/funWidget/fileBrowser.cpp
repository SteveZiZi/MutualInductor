#include "fileBrowser.h"

#include <QStandardItemModel>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QKeyEvent>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QScrollBar>
#include <QAction>

#include "src/CTTester/menuBar/menuBar.h"

#define REPORT_HEADER_H 25
#define REPORT_ITEM_H   30


CFileBrowser::CFileBrowser(const QString &path, QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{

    createReport();
    initMenu();
    _enumerateDir(path);


    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addWidget(m_report);

    setLayout(vlayout);
}

CFileBrowser::~CFileBrowser()
{

}

void CFileBrowser::keyPressEvent(QKeyEvent *eve)
{
    int keyvalue=eve->key();
    switch(keyvalue)
    {
    case Qt::Key_Escape:
        this->deleteLater();
        break;
    default:
        QWidget::keyPressEvent(eve);
        break;
    }
}

void CFileBrowser::reportSelectionChanged()
{

}

void CFileBrowser::createReport()
{
    m_report = new QTableWidget(this);
    m_report->setColumnCount(4);

    QHeaderView* hHeader = new QHeaderView(Qt::Horizontal, m_report);
    hHeader->setObjectName("fileBrowserTable");
    hHeader->setFixedHeight(REPORT_HEADER_H);
    hHeader->setHighlightSections(false);
    m_report->setHorizontalHeader(hHeader);
    m_report->setSelectionBehavior(QAbstractItemView::SelectRows);

    m_report->verticalHeader()->hide();
    m_report->horizontalScrollBar()->hide();
    //m_report->verticalScrollBar()->hide();

    m_report->setAlternatingRowColors(true);

    const QString captionName[] = {
        tr("fileName"),
        tr("fileSize"),
        tr("fileStyle"),
        tr("modifiedTime")
    };
    const int columnWidth[] = {
        250,
        118,
        80,
        215
    };
    for(int i = 0; i < m_report->columnCount(); i++) {
        QTableWidgetItem *harmonicValHeaderItem = new QTableWidgetItem(captionName[i]);
        harmonicValHeaderItem->setTextAlignment(Qt::AlignCenter);
        m_report->setHorizontalHeaderItem(i, harmonicValHeaderItem);
        m_report->setColumnWidth(i, columnWidth[i]);
    }
}

bool CFileBrowser::_enumerateDir(const QString &path)
{
    QDir dir(path);
    if (!dir.exists()) {
        return false;
    }

    m_fileList.clear();

    dir.setFilter(QDir::Dirs | QDir::NoSymLinks);
    m_fileList = dir.entryInfoList();

    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    m_fileList += dir.entryInfoList();

    m_report->clearContents();
    int row = 0;
    Q_FOREACH(const QFileInfo & fi, m_fileList) {
        if (fi.isDir()) {
            QString subDirName = fi.fileName();
            if (subDirName == "." || subDirName == "..") {
                continue;
            }
            m_report->insertRow(row);
            m_report->setItem(row, 0, new QTableWidgetItem("<"+subDirName+">"));
            m_report->setItem(row, 1, new QTableWidgetItem(""));
            m_report->setItem(row, 2, new QTableWidgetItem(tr("文件夹")));
            m_report->setItem(row, 3, new QTableWidgetItem(fi.lastModified().toString("yyyy-MM-dd hh:mm:ss")));
            m_report->setRowHeight(row, REPORT_ITEM_H);
            row++;
        }
        else if (fi.isFile()) {
            m_report->insertRow(row);
            m_report->setItem(row, 0, new QTableWidgetItem(fi.fileName()));
            if (fi.size() >= 2048)
                m_report->setItem(row, 1, new QTableWidgetItem(QString::number(fi.size()/1024)+"kB"));
            else
                m_report->setItem(row, 1, new QTableWidgetItem(QString::number(fi.size())+"B"));
            m_report->setItem(row, 2, new QTableWidgetItem(tr("文件")));
            m_report->setItem(row, 3, new QTableWidgetItem(fi.lastModified().toString("yyyy-MM-dd hh:mm:ss")));
            m_report->setRowHeight(row, REPORT_ITEM_H);
            row++;
        }
    }

    return true;
}

void CFileBrowser::initMenu()
{
//    this->addAction(new QAction(tr("开始试验"), this));
//    this->addAction(new QAction(tr("停止试验"), this));
//    this->addAction(new QAction(tr("导入参数"), this));
//    this->addAction(new QAction(tr("导出参数"), this));

//    GetMenuBar()->updateActions(this->actions());

//    QList<QAction*> actions = this->actions();
//    connect(actions[0], SIGNAL(triggered()), this, SLOT(onStartTest()));
//    connect(actions[1], SIGNAL(triggered()), this, SLOT(onStopTest()));
//    connect(actions[2], SIGNAL(triggered()), this, SLOT(loadParam()));
//    connect(actions[3], SIGNAL(triggered()), this, SLOT(saveParam()));

}
