#ifndef MLINEEDIT_H
#define MLINEEDIT_H

#include <QLineEdit>

class MLineEdit : public QLineEdit
{
public:
    explicit MLineEdit(QWidget* parent=0);
    explicit MLineEdit(const QString &, QWidget* parent=0);

protected:
    void keyPressEvent(QKeyEvent *eve);
};

#endif // MLINEEDIT_H
