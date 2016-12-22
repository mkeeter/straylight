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

bool Bridge::beginSheet(SheetIndex s)
{
    (void)s;
    return true;
}

void Bridge::endSheet()
{
}

void Bridge::instance(InstanceIndex i, const std::string& name)
{
}

void Bridge::input(CellIndex c, const std::string& name,
           const std::string& expr, bool valid,
           const std::string& val)
{
}

void Bridge::output(CellIndex c, const std::string& name,
            bool valid, const std::string& val)
{
}

void Bridge::cell(CellIndex c, const std::string& name,
                  const std::string& expr, Cell::Type type,
                  bool valid, const std::string& val)
{
}
