#include "graph/response.hpp"

namespace Graph {

//  Helper constructors
Response Response::ItemErased(const Env& e, const ItemIndex& i)
{
    return { ITEM_ERASED, e, "", "", i, false, Cell::UNKNOWN };
}

Response Response::IOErased(const CellKey& k)
{
    return { IO_DELETED, k.first, "", "", k.second, false, Cell::UNKNOWN };
}

Response Response::CellInserted(
        const CellKey& k, const std::string& name,
        const std::string& expr)
{
    return { CELL_INSERTED, k.first, name, expr, k.second, false, Cell::UNKNOWN };
}

Response Response::InstanceInserted(
        const Env& env, const InstanceIndex& i,
        const std::string& name, const std::string& sheet_name)
{
    return { INSTANCE_INSERTED, env, name, sheet_name, i, false, Cell::UNKNOWN };
}

Response Response::InputCreated(
        const CellKey& k, const std::string& name, const std::string& expr)
{
    return { INPUT_CREATED, k.first, name, expr, k.second, false, Cell::UNKNOWN };
}

Response Response::OutputCreated(const CellKey& k, const std::string& name)
{
    return { OUTPUT_CREATED, k.first, name, "", k.second, false, Cell::UNKNOWN };
}

Response Response::ExprChanged(const CellKey& c, const std::string& expr)
{
    return { EXPR_CHANGED, c.first, "", expr, c.second, false, Cell::UNKNOWN };
}

Response Response::InputChanged(const CellKey& c, const std::string& expr)
{
    return { INPUT_CHANGED, c.first, "", expr, c.second, false, Cell::UNKNOWN };
}

Response Response::ItemRenamed(
        const Env& env, const ItemIndex& i, const std::string& name)
{
    return { ITEM_RENAMED, env, name, "", i, false, Cell::UNKNOWN };
}

Response Response::SheetCreated(
        const Env& env, const SheetIndex& i, const std::string& name)
{
    return { SHEET_INSERTED, env, name, "", i, false, Cell::UNKNOWN };
}

Response Response::SheetRenamed(
        const Env& env, const SheetIndex& i, const std::string& name)
{
    return { SHEET_RENAMED, env, name, "", i, false, Cell::UNKNOWN };
}

Response Response::SheetErased(
        const Env& env, const SheetIndex& i)
{
    return { SHEET_ERASED, env, "", "", i, false, Cell::UNKNOWN };
}


Response Response::ValueChanged(
        const CellKey& k, const std::string& value, bool valid)
{
    return { VALUE_CHANGED, k.first, "", value, k.second, valid, Cell::UNKNOWN };
}

Response Response::CellTypeChanged(
        const CellKey& k, Cell::Type type)
{
    return { CELL_TYPE_CHANGED, k.first, "", "", k.second, false, type };
}


Response Response::Halt()
{
    return { HALT, {}, "", "", 0, false, Cell::UNKNOWN };
}

Response Response::ReservedWord(const std::string& value)
{
    return { RESERVED_WORD, {}, "", value, 0, false, Cell::UNKNOWN };
}

}   // namespace Graph
