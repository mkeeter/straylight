#include "graph/response.hpp"

namespace Graph {

//  Helper constructors
Response Response::CellErased(const CellKey& k)
{
    return { CELL_ERASED, "", "", k.second, false, k.first };
}

Response Response::InputErased(const CellKey& k)
{
    return { INPUT_DELETED, "", "", k.second, false, k.first };
}

Response Response::OutputErased(const CellKey& k)
{
    return { OUTPUT_DELETED, "", "", k.second, false, k.first };
}

Response Response::InstanceErased(const Env& env, const InstanceIndex& i)
{
    return { INSTANCE_ERASED, "", "", i, false, env };
}

Response Response::CellInserted(
        const CellKey& k, const std::string& name,
        const std::string& expr)
{
    return { CELL_INSERTED, name, expr, k.second, false, k.first };
}

Response Response::InstanceInserted(
        const Env& env, const InstanceIndex& i,
        const std::string& name, const std::string& sheet_name)
{
    return { INSTANCE_INSERTED, name, sheet_name, i, false, env };
}

Response Response::InputCreated(
        const CellKey& k, const std::string& name, const std::string& expr)
{
    return { INPUT_CREATED, name, expr, k.second, false, k.first };
}

Response Response::OutputCreated(const CellKey& k, const std::string& name)
{
    return { OUTPUT_CREATED, name, "", k.second, false, k.first };
}

Response Response::ExprChanged(const CellKey& c, const std::string& expr)
{
    return { EXPR_CHANGED, "", expr, c.second, false, c.first };
}

Response Response::InputChanged(const CellKey& c, const std::string& expr)
{
    return { INPUT_CHANGED, "", expr, c.second, false, c.first };
}

Response Response::ItemRenamed(
        const Env& env, const ItemIndex& i, const std::string& name)
{
    return { ITEM_RENAMED, name, "", i, false, env };
}

Response Response::SheetCreated(
        const Env& env, const SheetIndex& i, const std::string& name)
{
    return { SHEET_INSERTED, name, "", i, false, env };
}

Response Response::SheetRenamed(
        const Env& env, const SheetIndex& i, const std::string& name)
{
    return { SHEET_RENAMED, name, "", i, false, env };
}

Response Response::SheetErased(
        const Env& env, const SheetIndex& i)
{
    return { SHEET_ERASED, "", "", i, false, env };
}


Response Response::ValueChanged(
        const CellKey& k, const std::string& value, bool valid)
{
    return { VALUE_CHANGED, "", value, k.second, valid, k.first };
}

}   // namespace Graph
