#include "graph/response.hpp"

namespace Graph {

//  Helper constructors
Response Response::CellErased(const Env& e, const CellIndex& i)
{
    return { CELL_ERASED, e, "", "", i, 0, 0 };
}

Response Response::InstanceErased(const Env& e, const InstanceIndex& i)
{
    return { INSTANCE_ERASED, e, "", "", i, 0, 0 };
}

Response Response::IOErased(const CellKey& k)
{
    return { IO_DELETED, k.first, "", "", k.second, 0, 0 };
}

Response Response::CellInserted(
        const CellKey& k, const std::string& name,
        const std::string& expr)
{
    return { CELL_INSERTED, k.first, name, expr, k.second, 0, 0 };
}

Response Response::InstanceInserted(
        const Env& env, const InstanceIndex& i, const SheetIndex& sheet,
        const std::string& name, const std::string& sheet_name)
{
    return { INSTANCE_INSERTED, env, name, sheet_name, i, sheet, 0 };
}

Response Response::InstanceSheetRenamed(
        const Env& env, const InstanceIndex& i, const std::string& sheet_name)
{
    return { INSTANCE_SHEET_RENAMED, env, "", sheet_name, i, 0, 0 };
}

Response Response::InputCreated(
        const CellKey& k, const std::string& name, const std::string& expr)
{
    return { INPUT_CREATED, k.first, name, expr, k.second, 0, 0 };
}

Response Response::OutputCreated(const CellKey& k, const std::string& name)
{
    return { OUTPUT_CREATED, k.first, name, "", k.second, 0, 0 };
}

Response Response::ExprChanged(const CellKey& c, const std::string& expr)
{
    return { EXPR_CHANGED, c.first, "", expr, c.second, 0, 0 };
}

Response Response::InputChanged(const CellKey& c, const std::string& expr)
{
    return { INPUT_CHANGED, c.first, "", expr, c.second, 0, 0 };
}

Response Response::CellRenamed(
        const Env& env, const CellIndex& i, const std::string& name)
{
    return { CELL_RENAMED, env, name, "", i, 0, 0 };
}

Response Response::InstanceRenamed(
        const Env& env, const InstanceIndex& i, const std::string& name)
{
    return { INSTANCE_RENAMED, env, name, "", i, 0, 0 };
}

Response Response::SheetCreated(
        const Env& env, const SheetIndex& i, const std::string& name,
        bool editable, bool insertable)
{
    return { SHEET_INSERTED, env, name, "", i, 0,
        (uint8_t)((editable ? RESPONSE_FLAG_EDITABLE : 0) |
                  (insertable ? RESPONSE_FLAG_INSERTABLE : 0)) };
}

Response Response::SheetRenamed(
        const Env& env, const SheetIndex& i, const std::string& name)
{
    return { SHEET_RENAMED, env, name, "", i, 0, 0 };
}

Response Response::SheetErased(
        const Env& env, const SheetIndex& i)
{
    return { SHEET_ERASED, env, "", "", i, 0, 0 };
}


Response Response::ValueChanged(
        const CellKey& k, const std::string& value, bool valid)
{
    return { VALUE_CHANGED, k.first, "", value, k.second, 0, (uint8_t)(valid ? RESPONSE_FLAG_VALID : 0) };
}

Response Response::CellTypeChanged(
        const CellKey& k, Cell::Type type)
{
    uint8_t f;
    switch (type)
    {
        case Cell::INPUT:   f = RESPONSE_FLAG_TYPE_INPUT; break;
        case Cell::OUTPUT:  f = RESPONSE_FLAG_TYPE_OUTPUT; break;
        case Cell::BASIC:   f = RESPONSE_FLAG_TYPE_BASE; break;
        case Cell::UNKNOWN: f = RESPONSE_FLAG_TYPE_UNKNOWN; break;
    };

    return { CELL_TYPE_CHANGED, k.first, "", "", k.second, 0, f };
}

Response Response::ReservedWord(const std::string& value)
{
    return { RESERVED_WORD, {}, "", value, 0, 0, 0};
}

Response Response::ItemNameRegex(const std::string& r)
{
    return { ITEM_NAME_REGEX_GOOD, {}, "", r, 0, 0, 0 };
}

Response Response::SheetNameRegex(const std::string& r)
{
    return { SHEET_NAME_REGEX_GOOD, {}, "", r, 0, 0, 0 };
}

Response Response::ItemNameRegexBad(const std::string& r, const std::string& e)
{
    return { ITEM_NAME_REGEX_BAD, {}, e, r, 0, 0, 0 };
}

Response Response::SheetNameRegexBad(const std::string& r, const std::string& e)
{
    return { SHEET_NAME_REGEX_BAD, {}, e, r, 0, 0, 0 };
}

}   // namespace Graph
