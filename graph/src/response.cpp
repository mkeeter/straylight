#include "graph/response.hpp"

namespace Graph {

//  Helper constructors
Response Response::CellErased(SheetIndex s, CellIndex i)
{
    return Response { CELL_ERASED, s, i, 0, {}, "", "", 0 };
}

Response Response::InstanceErased(SheetIndex s, InstanceIndex i)
{
    return Response { INSTANCE_ERASED, s, i, 0, {}, "", "", 0 };
}

Response Response::IOErased(SheetIndex s, InstanceIndex i, CellIndex c)
{
    return Response { IO_DELETED, s, i, c, {}, "", "", 0 };
}

Response Response::CellInserted(
            SheetIndex s, CellIndex c,
            const std::string& name, const std::string& expr)
{
    return Response { CELL_INSERTED, s, c, 0, {}, name, expr, 0 };
}

Response Response::InstanceInserted(
            SheetIndex s, InstanceIndex i, SheetIndex t,
            const std::string& name, const std::string& sheet_name)
{
    return Response { INSTANCE_INSERTED, s, i, t, {}, name, sheet_name, 0 };
}

Response Response::InputCreated(
            SheetIndex s, InstanceIndex i, CellIndex c,
            const std::string& name, const std::string& expr)
{
    return Response { INPUT_CREATED, s, i, c, {}, name, expr, 0 };
}

Response Response::OutputCreated(
            SheetIndex s, InstanceIndex i, CellIndex c,
            const std::string& name)
{
    return Response { OUTPUT_CREATED, s, i, c, {}, name, "", 0 };
}

Response Response::ExprChanged(
            SheetIndex s, CellIndex c, const std::string& expr)
{
    return Response { EXPR_CHANGED, s, c, 0, {}, "", expr, 0 };
}

Response Response::InputChanged(
            SheetIndex s, InstanceIndex i, CellIndex c, const std::string& expr)
{
    return Response { INPUT_CHANGED, s, i, c, {}, "", expr, 0 };
}

Response Response::CellRenamed(
            SheetIndex s, CellIndex c, const std::string& name)
{
    return Response { CELL_RENAMED, s, c, 0, {}, name, "", 0 };
}

Response Response::InstanceRenamed(
            SheetIndex s, InstanceIndex i, const std::string& name)
{
    return Response { INSTANCE_RENAMED, s, i, 0, {}, name, "", 0 };
}

Response Response::SheetCreated(
            SheetIndex parent, SheetIndex s, const std::string& name)
{
    return Response { SHEET_INSERTED, parent, s, 0, {}, name, "", RESPONSE_FLAG_EDITABLE | RESPONSE_FLAG_INSERTABLE };
}

Response Response::SheetAvailable(
            SheetIndex parent, SheetIndex s, const std::string& name, bool insertable)
{
    return Response { SHEET_AVAILABLE, parent, s, 0, {}, name, "", (uint8_t)(insertable ? RESPONSE_FLAG_INSERTABLE : 0) };
}

Response Response::SheetUnavailable(
            SheetIndex parent, SheetIndex s)
{
    return Response { SHEET_UNAVAILABLE, parent, s, 0, {}, "", "", 0 };
}

Response Response::SheetRenamed(
            SheetIndex parent, SheetIndex i, const std::string& name)
{
    return Response { SHEET_RENAMED, parent, i, 0, {}, name, "", 0 };
}

Response Response::SheetErased(
            SheetIndex parent, SheetIndex i)
{
    return Response { SHEET_ERASED, parent, i, 0, {}, "", "", 0 };
}

Response Response::ValueChanged(
            SheetIndex s, const CellKey& k,
            const std::string& value, bool valid)
{
    return Response { VALUE_CHANGED, s, k.second, 0, k.first, "", value,
        (uint8_t)(valid ? RESPONSE_FLAG_VALID : 0)};
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
    return Response { CELL_TYPE_CHANGED, s, c, 0, {}, "", "", f };
}

Response Response::InstanceSheetRenamed(
            SheetIndex s, InstanceIndex i, const std::string& sheet_name)
{
    // TODO: send env as well?
    return Response { INSTANCE_SHEET_RENAMED, s, i, 0, {}, sheet_name, "", 0 };
}

////////////////////////////////////////////////////////////////////////////////

Response Response::ReservedWord(const std::string& r)
{
    return { RESERVED_WORD, 0, 0, 0, {}, "", r, 0 };
}

Response Response::ItemNameRegex(const std::string& r)
{
    return { ITEM_NAME_REGEX_GOOD, 0, 0, 0, {}, "", r, 0 };
}

Response Response::SheetNameRegex(const std::string& r)
{
    return { SHEET_NAME_REGEX_GOOD, 0, 0, 0, {}, "", r, 0 };
}

Response Response::ItemNameRegexBad(const std::string& r, const std::string& e)
{
    return { ITEM_NAME_REGEX_BAD, 0, 0, 0, {}, e, r, 0 };
}

Response Response::SheetNameRegexBad(const std::string& r, const std::string& e)
{
    return { SHEET_NAME_REGEX_BAD, 0, 0, 0, {}, e, r, 0 };
}

}   // namespace Graph
