#include "graph/response.hpp"

namespace Graph {

//  Helper constructors
Response Response::CellErased(SheetIndex s, CellIndex i)
{
    return Response { CELL_ERASED, s, i, 0, {}, "", "", 0, nullptr };
}

Response Response::InstanceErased(SheetIndex s, InstanceIndex i)
{
    return Response { INSTANCE_ERASED, s, i, 0, {}, "", "", 0, nullptr };
}

Response Response::IOErased(SheetIndex s, InstanceIndex i, CellIndex c)
{
    return Response { IO_DELETED, s, i, c, {}, "", "", 0, nullptr };
}

Response Response::IORenamed(SheetIndex s, InstanceIndex i, CellIndex c,
        const std::string& name)
{
    return Response { IO_RENAMED, s, i, c, {}, name, "", 0, nullptr };
}

Response Response::CellInserted(
            SheetIndex s, CellIndex c,
            const std::string& name, const std::string& expr)
{
    return Response { CELL_INSERTED, s, c, 0, {}, name, expr, 0, nullptr };
}

Response Response::InstanceInserted(
            SheetIndex s, InstanceIndex i, SheetIndex t,
            const std::string& name, const std::string& sheet_name)
{
    return Response { INSTANCE_INSERTED, s, i, t, {}, name, sheet_name, 0, nullptr };
}

Response Response::InputCreated(
            SheetIndex s, InstanceIndex i, CellIndex c,
            const std::string& name, const std::string& expr)
{
    return Response { IO_INPUT_CREATED, s, i, c, {}, name, expr, 0, nullptr };
}

Response Response::OutputCreated(
            SheetIndex s, InstanceIndex i, CellIndex c,
            const std::string& name)
{
    return Response { IO_OUTPUT_CREATED, s, i, c, {}, name, "", 0, nullptr };
}

Response Response::ExprChanged(
            SheetIndex s, CellIndex c, const std::string& expr)
{
    return Response { EXPR_CHANGED, s, c, 0, {}, "", expr, 0, nullptr };
}

Response Response::InputExprChanged(
            SheetIndex s, InstanceIndex i, CellIndex c, const std::string& expr)
{
    return Response { IO_EXPR_CHANGED, s, i, c, {}, "", expr, 0, nullptr };
}

Response Response::IOValueChanged(
            SheetIndex s, InstanceIndex i, CellIndex c,
            const Env& env, const std::string& val, bool valid,
            Escaped* value)
{
    return Response { IO_VALUE_CHANGED, s, i, c, env, "", val,
        (uint8_t)(valid ? RESPONSE_FLAG_VALID : 0), value };
}

Response Response::CellRenamed(
            SheetIndex s, CellIndex c, const std::string& name)
{
    return Response { CELL_RENAMED, s, c, 0, {}, name, "", 0, nullptr };
}

Response Response::InstanceRenamed(
            SheetIndex s, InstanceIndex i, const std::string& name)
{
    return Response { INSTANCE_RENAMED, s, i, 0, {}, name, "", 0, nullptr };
}

Response Response::SheetCreated(
            SheetIndex parent, SheetIndex s, const std::string& name)
{
    return Response { SHEET_INSERTED, parent, s, 0, {}, name, "",
        RESPONSE_FLAG_EDITABLE | RESPONSE_FLAG_INSERTABLE, nullptr };
}

Response Response::SheetAvailable(
            SheetIndex parent, SheetIndex s, const std::string& name, bool insertable)
{
    return Response { SHEET_AVAILABLE, parent, s, 0, {}, name, "",
        (uint8_t)(insertable ? RESPONSE_FLAG_INSERTABLE : 0), nullptr };
}

Response Response::SheetUnavailable(
            SheetIndex parent, SheetIndex s)
{
    return Response { SHEET_UNAVAILABLE, parent, s, 0, {}, "", "", 0, nullptr };
}

Response Response::SheetRenamed(
            SheetIndex parent, SheetIndex i, const std::string& name)
{
    return Response { SHEET_RENAMED, parent, i, 0, {}, name, "", 0, nullptr };
}

Response Response::SheetErased(
            SheetIndex parent, SheetIndex i)
{
    return Response { SHEET_ERASED, parent, i, 0, {}, "", "", 0, nullptr };
}

Response Response::ValueChanged(
            SheetIndex s, const CellKey& k,
            const std::string& v, bool valid,
            Escaped* value)
{
    return Response { VALUE_CHANGED, s, k.second, 0, k.first, "", v,
        (uint8_t)(valid ? RESPONSE_FLAG_VALID : 0), value };
}

Response Response::ValueErased(SheetIndex s, const CellKey& k)
{
    return Response { VALUE_ERASED, s, k.second, 0, k.first, "", "", 0, nullptr };
}

Response Response::IsEndpointChanged(
            SheetIndex s, const CellKey& k, bool endpoint)
{
    return Response { IS_ENDPOINT_CHANGED, s, k.second, 0, k.first, "", "",
        (uint8_t)(endpoint ? RESPONSE_FLAG_ENDPOINT : 0), nullptr };
}

Response Response::CellTypeChanged(SheetIndex s, CellIndex c, Cell::Type type)
{
    uint8_t f;
    switch (type)
    {
        case Cell::INPUT:   f = RESPONSE_FLAG_TYPE_INPUT; break;
        case Cell::OUTPUT:  f = RESPONSE_FLAG_TYPE_OUTPUT; break;
        case Cell::BASIC:   f = RESPONSE_FLAG_TYPE_BASE; break;
        case Cell::UNKNOWN: f = RESPONSE_FLAG_TYPE_UNKNOWN; break;
    };
    return Response { CELL_TYPE_CHANGED, s, c, 0, {}, "", "", f, nullptr };
}

Response Response::InstanceSheetRenamed(
            SheetIndex s, InstanceIndex i, const std::string& sheet_name)
{
    return Response { INSTANCE_SHEET_RENAMED, s, i, 0, {}, sheet_name, "", 0, nullptr };
}

Response Response::Serialized(const std::string& result)
{
    return Response { SERIALIZED, 0, 0, 0, {}, "", result, 0, nullptr };
}

Response Response::Deserialized(const std::string& error)
{
    return Response { DESERIALIZED, 0, 0, 0, {}, "", error, 0, nullptr };
}

////////////////////////////////////////////////////////////////////////////////

Response Response::ReservedWord(const std::string& r)
{
    return { RESERVED_WORD, 0, 0, 0, {}, "", r, 0, nullptr };
}

Response Response::ItemNameRegex(const std::string& r)
{
    return { ITEM_NAME_REGEX_GOOD, 0, 0, 0, {}, "", r, 0, nullptr };
}

Response Response::SheetNameRegex(const std::string& r)
{
    return { SHEET_NAME_REGEX_GOOD, 0, 0, 0, {}, "", r, 0, nullptr };
}

Response Response::ItemNameRegexBad(const std::string& r, const std::string& e)
{
    return { ITEM_NAME_REGEX_BAD, 0, 0, 0, {}, e, r, 0, nullptr };
}

Response Response::SheetNameRegexBad(const std::string& r, const std::string& e)
{
    return { SHEET_NAME_REGEX_BAD, 0, 0, 0, {}, e, r, 0, nullptr };
}

}   // namespace Graph
