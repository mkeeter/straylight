#include "graph/response.hpp"

namespace Graph {

//  Helper constructors
Response Response::ItemErased(const Env& e, const ItemIndex& i)
{
    return { ITEM_ERASED, e, "", "", i, false };
}

Response Response::IOErased(const CellKey& k)
{
    return { IO_DELETED, k.first, "", "", k.second, false };
}

Response Response::CellInserted(
        const CellKey& k, const std::string& name,
        const std::string& expr)
{
    return { CELL_INSERTED, k.first, name, expr, k.second, false };
}

Response Response::InstanceInserted(
        const Env& env, const InstanceIndex& i,
        const std::string& name, const std::string& sheet_name)
{
    return { INSTANCE_INSERTED, env, name, sheet_name, i, false };
}

Response Response::InputCreated(
        const CellKey& k, const std::string& name, const std::string& expr)
{
    return { INPUT_CREATED, k.first, name, expr, k.second, false };
}

Response Response::OutputCreated(const CellKey& k, const std::string& name)
{
    return { OUTPUT_CREATED, k.first, name, "", k.second, false };
}

Response Response::ExprChanged(const CellKey& c, const std::string& expr)
{
    return { EXPR_CHANGED, c.first, "", expr, c.second, false };
}

Response Response::InputChanged(const CellKey& c, const std::string& expr)
{
    return { INPUT_CHANGED, c.first, "", expr, c.second, false };
}

Response Response::ItemRenamed(
        const Env& env, const ItemIndex& i, const std::string& name)
{
    return { ITEM_RENAMED, env, name, "", i, false };
}

Response Response::SheetCreated(
        const Env& env, const SheetIndex& i, const std::string& name)
{
    return { SHEET_INSERTED, env, name, "", i, false };
}

Response Response::SheetRenamed(
        const Env& env, const SheetIndex& i, const std::string& name)
{
    return { SHEET_RENAMED, env, name, "", i, false };
}

Response Response::SheetErased(
        const Env& env, const SheetIndex& i)
{
    return { SHEET_ERASED, env, "", "", i, false };
}


Response Response::ValueChanged(
        const CellKey& k, const std::string& value, bool valid)
{
    return { VALUE_CHANGED, k.first, "", value, k.second, valid };
}

}   // namespace Graph
