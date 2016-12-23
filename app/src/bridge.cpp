#include "bridge.hpp"

Bridge* Bridge::_instance = nullptr;

Bridge::Bridge()
    : bts(this)
{
    r.insertCell(0, "omg", "wtf");
    r.insertCell(0, "bbq", "(+ 1 2)");
}

QString Bridge::checkName(int sheet_index, QString name) const
{
    std::string out;
    r.checkName(sheet_index, name.toStdString(), &out);
    return QString::fromStdString(out);
}

QString Bridge::checkRename(int item_index, QString name) const
{
    const auto& current_name = r.nameOf(item_index);
    if (current_name == name.toStdString())
    {
        return "";
    }

    auto sheet = r.parentSheet(item_index);
    return checkName(sheet.i, name);
}

void Bridge::renameItem(int item_index, QString name)
{
    r.renameItem(item_index, name.toStdString());
    sync();
}

void Bridge::insertCell(int sheet_index, const QString& name)
{
    r.insertCell(sheet_index, name.toStdString());
    sync();
}

void Bridge::setExpr(int cell_index, const QString& expr)
{
    if (r.setExpr(cell_index, expr.toStdString()))
    {
        sync();
    }
}

void Bridge::eraseCell(int cell_index)
{
    r.eraseCell(cell_index);
    sync();
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
