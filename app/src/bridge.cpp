#include "bridge.hpp"

Bridge* Bridge::_instance = nullptr;

bool Bridge::checkName(QString name) const
{
    return name.count("x");
}

QObject* Bridge::singleton(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    if (_instance == nullptr)
    {
        _instance = new Bridge();
    }
    return _instance;
}

Root* Bridge::root()
{
    if (_instance == nullptr)
    {
        _instance = new Bridge();
    }
    return &_instance->r;
}

void Bridge::sync()
{
    r.serialize(&bts);
}

////////////////////////////////////////////////////////////////////////////////

bool Bridge::BridgeTreeSerializer::beginSheet(SheetIndex s)
{
    parent->beginSheet(s.i);
    return true;
}

void Bridge::BridgeTreeSerializer::endSheet()
{
    parent->endSheet();
}

void Bridge::BridgeTreeSerializer::instance(InstanceIndex i, const std::string& name)
{
    parent->instance(i.i, QString::fromStdString(name));
}

void Bridge::BridgeTreeSerializer::input(CellIndex c, const std::string& name,
           const std::string& expr, bool valid,
           const std::string& val)
{
    parent->input(c.i, QString::fromStdString(name),
                  QString::fromStdString(expr), valid,
                  QString::fromStdString(val));
}

void Bridge::BridgeTreeSerializer::output(CellIndex c, const std::string& name,
            bool valid, const std::string& val)
{
    parent->output(c.i, QString::fromStdString(name),
                   valid, QString::fromStdString(val));
}

void Bridge::BridgeTreeSerializer::cell(CellIndex c, const std::string& name,
                  const std::string& expr, Cell::Type type,
                  bool valid, const std::string& val)
{
    parent->cell(c.i, QString::fromStdString(name),
                 QString::fromStdString(expr), type,
                 valid, QString::fromStdString(val));
}
