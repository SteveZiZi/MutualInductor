#include "QFileDlg.h"
#include <QTableView>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QHeaderView>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QEvent>
#include <QDebug>

#define QFILEDLG_WIN_W      800
#define QFILEDLG_WIN_H      480
#define QFILEDLG_WIN_MARGIN 20

class QMyFileSystemModel :public QFileSystemModel
{
public:
    QMyFileSystemModel(QObject * parent = 0) :QFileSystemModel(parent)
    {
    }
protected:
    QVariant headerData(int section, Qt::Orientation orientation, int role) const
    {
        // 修改QFileSystemModel标题栏的显示文本
        if (orientation == Qt::Horizontal) {
            if (role != Qt::DisplayRole)
                return QVariant();
            switch (section) {
            case 0: return tr("名称");
            case 1: return tr("大小");
            case 2: return tr("类型");
            case 3: return tr("修改日期");
            default: return QVariant();
            }
        }
        return QAbstractItemModel::headerData(section, orientation, role);
    }
};


QFileDlg::QFileDlg(QWidget *parent/* = 0*/)
    : QDialog(parent)
{
    // 创建控件
    m_pFileSysModel = new QMyFileSystemModel();
    m_pTitleLabel = new QLabel(tr("请选择文件:"), this);
    m_pTableView = new QTableView(this);
    m_pFileNameLabel = new QLabel(tr("文件名:"), this);
    m_pFileNameEdit = new QLineEdit(this);
    m_pFilePathLabel = new QLabel(tr("文件路径:"), this);
    m_pFilePathEdit = new QLineEdit(this);
    m_pOkButton = new QPushButton(tr("确定"), this);
    m_pCancelButton = new QPushButton(tr("取消"), this);

    // 文件路径不可编辑
    m_pFilePathEdit->setDisabled(true);

    // 初始化QFileSystemModel
    m_pFileSysModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDot);
    m_pFileSysModel->setNameFilterDisables(false);

    // 创建一个网格布局管理器对象，用于底部按钮和edit框
    m_pFileInfoLayout = new QGridLayout();
    m_pFileInfoLayout->addWidget(m_pFileNameLabel, 0, 0, 1, 1);
    m_pFileInfoLayout->addWidget(m_pFileNameEdit, 0, 1, 1, 1);
    m_pFileInfoLayout->addWidget(m_pOkButton, 0, 3, 1, 1);
    m_pFileInfoLayout->addWidget(m_pFilePathLabel, 1, 0, 1, 1);
    m_pFileInfoLayout->addWidget(m_pFilePathEdit, 1, 1, 1, 1);
    m_pFileInfoLayout->addWidget(m_pCancelButton, 1, 3, 1, 1);
    m_pFileInfoLayout->setColumnStretch(0, 1);
    m_pFileInfoLayout->setColumnStretch(1, 5);
    m_pFileInfoLayout->setColumnStretch(2, 1);
    m_pFileInfoLayout->setColumnStretch(3, 1);

    // 创建一个垂直布局管理器对象，用于整体布局
    m_pLayout = new QVBoxLayout();
    m_pLayout->setMargin(QFILEDLG_WIN_MARGIN);
    m_pLayout->addWidget(m_pTitleLabel);
    m_pLayout->addWidget(m_pTableView);
    m_pLayout->addLayout(m_pFileInfoLayout);
    setLayout(m_pLayout);

    // 初始化表格
    m_pTableView->setModel(m_pFileSysModel);
    m_pTableView->setShowGrid(false);
    m_pTableView->setSortingEnabled(true);
    m_pTableView->verticalHeader()->hide();
    m_pTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pTableView->setColumnWidth(0, (QFILEDLG_WIN_W-QFILEDLG_WIN_MARGIN*2)/2);
    m_pTableView->setColumnWidth(1, (QFILEDLG_WIN_W-QFILEDLG_WIN_MARGIN*2)/8);
    m_pTableView->setColumnWidth(2, (QFILEDLG_WIN_W-QFILEDLG_WIN_MARGIN*2)/8);
    m_pTableView->setColumnWidth(3, (QFILEDLG_WIN_W-QFILEDLG_WIN_MARGIN*2)/4 - 2);
    m_pTableView->setFont(QFont("Microsoft YaHei", 12));

    // 信号链接
    connect(m_pOkButton, SIGNAL(clicked()), this, SLOT(_onOkClick()));
    connect(m_pCancelButton, SIGNAL(clicked()), this, SLOT(_onCancelClick()));
    connect(m_pTableView, SIGNAL(clicked(QModelIndex)), this, SLOT(_onTableItemClick(QModelIndex)));

    // 修改窗口样式
    resize(QFILEDLG_WIN_W, QFILEDLG_WIN_H);
    setWindowFlags(Qt::FramelessWindowHint);
}

QFileDlg::~QFileDlg()
{
    delete m_pCancelButton;
    delete m_pOkButton;
    delete m_pFilePathEdit;
    delete m_pFilePathLabel;
    delete m_pFileNameEdit;
    delete m_pFileNameLabel;
    delete m_pTableView;
    delete m_pTitleLabel;
    delete m_pFileSysModel;
    delete m_pFileInfoLayout;
    delete m_pLayout;
}

void QFileDlg::setFilter(QDir::Filters filters)
{
    Q_ASSERT(m_pFileSysModel);
    m_pFileSysModel->setFilter(filters);
}

void QFileDlg::setNameFilters(const QStringList& filters)
{
    Q_ASSERT(m_pFileSysModel);
    m_pFileSysModel->setNameFilters(filters);
}

void QFileDlg::setRootPath(const QString& path)
{
    Q_ASSERT(m_pFileSysModel);
    m_pFileSysModel->setRootPath(path);
}

void QFileDlg::setCurrentPath(const QString& path)
{
    Q_ASSERT(m_pFileSysModel);
    Q_ASSERT(m_pTableView);
    m_pTableView->setRootIndex(m_pFileSysModel->index(path));
    if (path.compare(m_pFileSysModel->rootPath()) == 0) {
        m_pFileSysModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    }
    else {
        m_pFileSysModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDot);
    }
    m_pFilePathEdit->setText(QDir(path).absolutePath());
}

QString QFileDlg::getFilePath()
{
    return (getFolderPath() + "/" + getFileName());
}

QString QFileDlg::getFileName()
{
    Q_ASSERT(m_pFileNameEdit);
    return m_pFileNameEdit->text();
}

QString QFileDlg::getFolderPath()
{
    Q_ASSERT(m_pFileSysModel);
    Q_ASSERT(m_pTableView);
    QModelIndex idx = m_pTableView->currentIndex();
    if (idx.isValid()) {
        return (m_pFileSysModel->fileInfo(idx).absolutePath());
    }
    return m_pFileSysModel->rootDirectory().absolutePath();
}

void QFileDlg::_onOkClick()
{
    if (m_pFileNameEdit->text().isEmpty()) {
        QMessageBox dlg;
        dlg.setIconPixmap(QPixmap(":/resource/images/warn.png"));
        dlg.setWindowTitle(tr("提示"));
        dlg.setText(tr("请指定文件名!"));
        dlg.exec();
    }
    else {
        QDialog::done(QDialog::Accepted);
    }
}

void QFileDlg::_onCancelClick()
{
    QDialog::done(QDialog::Rejected);
}

void QFileDlg::_onTableItemClick(const QModelIndex & index)
{
    if (m_pFileSysModel->isDir(index))
    {
        QString rootPath = QDir(m_pFileSysModel->rootPath()).absolutePath();
        QString idxPath = QDir(m_pFileSysModel->filePath(index)).absolutePath();
        if (idxPath.compare(rootPath) == 0) {
            m_pFileSysModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
        } else {
            m_pFileSysModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDot);
        }
        m_pTableView->setRootIndex(m_pFileSysModel->index(idxPath));
        m_pFilePathEdit->setText(idxPath);
    }
    else
    {
        m_pFileNameEdit->setText(m_pFileSysModel->fileName(index));
        m_pFilePathEdit->setText(m_pFileSysModel->fileInfo(index).absoluteDir().absolutePath());
    }
    
}
