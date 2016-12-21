#include <QObject>

class NameValidator : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE bool checkName(QString name) const;
};
