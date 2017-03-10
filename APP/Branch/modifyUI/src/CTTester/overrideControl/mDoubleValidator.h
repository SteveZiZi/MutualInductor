#ifndef MDOUBLEVALIDATOR_H
#define MDOUBLEVALIDATOR_H

/*解决QDoubleValidator最大值不生效的问题*/

#include <QDoubleValidator>

class MDoubleValidator : public QDoubleValidator
{
public:
    MDoubleValidator(double bottom, double top, int decimals, QObject *parent = 0);

    QValidator::State validate(QString &input, int &pos) const;
};

#endif // MDOUBLEVALIDATOR_H
