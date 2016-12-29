#include "bridge.hpp"

Bridge* Bridge::_instance = nullptr;

Bridge::Bridge()
    : bts(this)
{
    r.insertCell(0, "omg", "wtf");
    r.insertCell(0, "bbq", "(+ 1 2)");
    r.insertCell(0, "whoa", "(+ 1 2)");

    auto s = r.insertSheet(0, "sheet");
    r.insertCell(s, "in", "(input 12)");
    r.insertCell(s, "out", "(output (+ 1 (in)))");
    r.insertInstance(0, "instance", s);
}

QString Bridge::checkItemName(int sheet_index, QString name) const
{
    std::string out;
    r.checkItemName(sheet_index, name.toStdString(), &out);
    return QString::fromStdString(out);
}

QString Bridge::checkItemRename(int item_index, QString name) const
{
    const auto& current_name = r.itemName(item_index);
    if (current_name == name.toStdString())
    {
        return "";
    }

    auto sheet = r.itemParent(item_index);
    return checkItemName(sheet.i, name);
}

void Bridge::renameItem(int item_index, QString name)
{
    r.renameItem(item_index, name.toStdString());
    sync();
}

////////////////////////////////////////////////////////////////////////////////

QString Bridge::checkSheetName(int parent_sheet, QString name) const
{
    std::string out;
    r.checkSheetName(parent_sheet, name.toStdString(), &out);
    return QString::fromStdString(out);
}

QString Bridge::checkSheetRename(int sheet_index, QString name) const
{
    const auto& current_name = r.sheetName(sheet_index);
    if (current_name == name.toStdString())
    {
        return "";
    }

    auto sheet = r.sheetParent(sheet_index);
    return checkSheetName(sheet.i, name);
}

void Bridge::renameSheet(int sheet_index, QString name)
{
    r.renameSheet(sheet_index, name.toStdString());
    sync();
}

void Bridge::insertSheet(int sheet_index, QString name)
{
    r.insertSheet(sheet_index, name.toStdString());
    sync();
}

void Bridge::eraseSheet(int sheet_index)
{
    r.eraseSheet(SheetIndex(sheet_index));
    sync();
}

////////////////////////////////////////////////////////////////////////////////

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

void Bridge::setInput(int instance_index, int cell_index,
                      const QString& expr)
{
    if (r.setInput(InstanceIndex(instance_index), CellIndex(cell_index),
                   expr.toStdString()))
    {
        sync();
    }
}

void Bridge::eraseCell(int cell_index)
{
    r.eraseCell(cell_index);
    sync();
}

void Bridge::insertInstance(int parent_sheet_index, QString name,
                            int target_sheet_index)
{
    r.insertInstance(parent_sheet_index, name.toStdString(),
                     target_sheet_index);
    sync();
}

void Bridge::eraseInstance(int instance_index)
{
    r.eraseInstance(instance_index);
    sync();
}

int Bridge::sheetOf(int instance_index) const
{
    return r.instanceSheet(InstanceIndex(instance_index)).i;
}

QString Bridge::nextItemName(int sheet_index) const
{
    return QString::fromStdString(
            r.nextItemName(SheetIndex(sheet_index), "i"));
}

QString Bridge::nextSheetName(int sheet_index) const
{
    return QString::fromStdString(
            r.nextSheetName(SheetIndex(sheet_index), "s"));
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

bool Bridge::BridgeTreeSerializer::push(InstanceIndex i,
                                        const std::string& instance_name,
                                        const std::string& sheet_name)
{
    parent->push(i.i, QString::fromStdString(instance_name),
                 QString::fromStdString(sheet_name));
    return true;
}

void Bridge::BridgeTreeSerializer::pop()
{
    parent->pop();
}

void Bridge::BridgeTreeSerializer::instance(
        InstanceIndex i, const std::string& name, const std::string& sheet)
{
    parent->instance(i.i, QString::fromStdString(name),
            QString::fromStdString(sheet));
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

void Bridge::BridgeTreeSerializer::sheet(SheetIndex s, const std::string& name,
                bool editable, bool insertable)
{
    parent->sheet(s.i, QString::fromStdString(name), editable, insertable);
}
