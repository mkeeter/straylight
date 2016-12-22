#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>

#include "graph/root.hpp"

class Bridge : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE bool checkName(QString name) const;

    static QObject* singleton(QQmlEngine *engine, QJSEngine *scriptEngine);

protected:
    Root graph;
};
