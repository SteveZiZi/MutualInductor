#ifndef QFILE_DLG_H__
#define QFILE_DLG_H__

#include <QDialog>
#include <QFileSystemModel>

class QTableView;
class QLabel;
class QLineEdit;
class QPushButton;
class QGridLayout;
class QVBoxLayout;

class QFileDlg : public QDialog
{
    Q_OBJECT
public:
    QFileDlg(QWidget *parent = 0);
    virtual ~QFileDlg();

    //
    // @brief
    //     设置显示元素（如目录、文件等，同QFileSystemModel::setFilter）
    //
    void setFilter(QDir::Filters filters);

    //
    // @brief
    //     设置显示元素的类型（如指定后缀名的文件，同QFileSystemModel::setNameFilters)
    //
    void setNameFilters(const QStringList& filters);

    //
    // @brief
    //     设置根目录(即显示的最高层级的目录)
    //
    void setRootPath(const QString& path);

    //
    // @brief
    //     设置当前目录/文件的路径(需在根目录之下)
    //
    void setCurrentPath(const QString& path);

    //
    // @brief
    //     获取选中文件的全路径
    //     "c:\example\test.xml"  -->  "c:\example\test.xml"
    //
    QString getFilePath();

    //
    // @brief
    //     获取选中文件的文件名
    //     "c:\example\test.xml"  -->  "test.xml"
    //
    QString getFileName();

    //
    // @brief
    //     获取选中文件的所在的目录
    //     "c:\example\test.xml"  -->  "x:\example"
    //
    QString getFolderPath();

protected Q_SLOTS:
    //
    // @brief
    //     消息处理
    //
    void _onOkClick();
    void _onCancelClick();
    void _onTableItemClick(const QModelIndex & index);

private:
    QFileSystemModel*   m_pFileSysModel;              // 文件系统模型
    QLabel*             m_pTitleLabel;                // 标题
    QTableView*         m_pTableView;                 // 文件浏览表
    QLabel*             m_pFileNameLabel;             // 文件名Label
    QLineEdit*          m_pFileNameEdit;              // 文件名
    QLabel*             m_pFilePathLabel;             // 文件路径Label
    QLineEdit*          m_pFilePathEdit;              // 文件路径
    QPushButton*        m_pOkButton;                  // 确认按钮
    QPushButton*        m_pCancelButton;              // 取消按钮
    QGridLayout*        m_pFileInfoLayout;            // 文件信息布局
    QVBoxLayout*        m_pLayout;                    // 总体布局
};

#endif
