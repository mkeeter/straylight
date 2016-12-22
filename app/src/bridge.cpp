#include "bridge.hpp"

bool Bridge::checkName(QString name) const
{
    return name.count("x");
}


QObject* Bridge::singleton(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new Bridge();
}
