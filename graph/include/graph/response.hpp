#pragma once

#include <string>

#include "graph/types/index.hpp"
#include "graph/types/keys.hpp"
#include "graph/types/env.hpp"
#include "graph/types/cell.hpp"

namespace Graph {

class Escaped;

struct Response
{
    enum {
        SHEET_RENAMED,
        SHEET_INSERTED,
        SHEET_AVAILABLE,
        SHEET_UNAVAILABLE,
        SHEET_ERASED,

        CELL_RENAMED,
        INSTANCE_RENAMED,
        INSTANCE_SHEET_RENAMED,
        CELL_INSERTED,
        CELL_TYPE_CHANGED,
        EXPR_CHANGED,
        INSTANCE_INSERTED,
        CELL_ERASED,
        INSTANCE_ERASED,

        VALUE_CHANGED,
        VALUE_ERASED,

        IO_INPUT_CREATED,
        IO_OUTPUT_CREATED,
        IO_DELETED,
        IO_EXPR_CHANGED,
        IO_VALUE_CHANGED,
        IO_RENAMED,

        IS_ENDPOINT_CHANGED,

        ITEM_NAME_REGEX_BAD,
        SHEET_NAME_REGEX_BAD,
        ITEM_NAME_REGEX_GOOD,
        SHEET_NAME_REGEX_GOOD,
        RESERVED_WORD,
        RESET_UNDO_QUEUE,
        UNDO_READY,
        REDO_READY,
        UNDO_NOT_READY,
        REDO_NOT_READY,

        SERIALIZED,
        DESERIALIZED,

        CLEAR,
    } op;

    // What sheet does this response modify?
    SheetIndex sheet;

    // What is the target item?
    ItemIndex target;

    // What's the secondary target, e.g. when inserting instances?
    ItemIndex other;

    // Used in value change operations
    Env env;

    // Enough variables to handle all the possible operations
    std::string name;
    std::string expr;

    enum ResponseFlag {
        RESPONSE_FLAG_VALID         = (1 << 0),
        RESPONSE_FLAG_EDITABLE      = (1 << 1),
        RESPONSE_FLAG_INSERTABLE    = (1 << 2),
        RESPONSE_FLAG_TYPE_INPUT    = (1 << 3),
        RESPONSE_FLAG_TYPE_OUTPUT   = (1 << 4),
        RESPONSE_FLAG_TYPE_BASE     = (1 << 5),
        RESPONSE_FLAG_TYPE_UNKNOWN  = (1 << 6),
        RESPONSE_FLAG_ENDPOINT      = (1 << 7),
    };
    uint8_t flags;
    Escaped* value;

    //  Helper constructors
    static Response CellErased(SheetIndex s, CellIndex i);
    static Response InstanceErased(SheetIndex s, InstanceIndex i);
    static Response IOErased(SheetIndex s, InstanceIndex i, CellIndex c);
    static Response IORenamed(
            SheetIndex s, InstanceIndex i, CellIndex c,
            const std::string& name);
    static Response CellInserted(
            SheetIndex s, CellIndex c,
            const std::string& name, const std::string& expr);
    static Response InstanceInserted(
            SheetIndex s, InstanceIndex i, SheetIndex t,
            const std::string& name, const std::string& sheet_name);
    static Response InputCreated(
            SheetIndex s, InstanceIndex i, CellIndex c,
            const std::string& name, const std::string& expr);
    static Response OutputCreated(
            SheetIndex s, InstanceIndex i, CellIndex c,
            const std::string& name);
    static Response ExprChanged(
            SheetIndex s, CellIndex c, const std::string& expr);
    static Response InputExprChanged(
            SheetIndex s, InstanceIndex i, CellIndex c, const std::string& expr);
    static Response IOValueChanged(
            SheetIndex s, InstanceIndex i, CellIndex c,
            const Graph::Env& env, const std::string& val, bool valid,
            Escaped* value);
    static Response CellRenamed(
            SheetIndex s, CellIndex c, const std::string& name);
    static Response InstanceRenamed(
            SheetIndex s, InstanceIndex i, const std::string& name);
    static Response SheetCreated(
            SheetIndex parent, SheetIndex s , const std::string& name);
    static Response SheetAvailable(
            SheetIndex parent, SheetIndex s,
            const std::string& name, bool insertable);
    static Response SheetUnavailable(
            SheetIndex parent, SheetIndex s);
    static Response SheetRenamed(
            SheetIndex parent, SheetIndex i, const std::string& name);
    static Response SheetErased(
            SheetIndex parent, SheetIndex i);
    static Response ValueChanged(
            SheetIndex i, const CellKey& k,
            const std::string& val, bool valid,
            Escaped* value);
    static Response ValueErased(SheetIndex i, const CellKey& k);
    static Response IsEndpointChanged(
            SheetIndex s, const CellKey& k, bool endpoint);
    static Response CellTypeChanged(SheetIndex s, CellIndex c, Cell::Type type);
    static Response InstanceSheetRenamed(
            SheetIndex s, InstanceIndex i, const std::string& sheet_name);
    static Response Serialized(const std::string& result);
    static Response Deserialized(const std::string& error);

    // Other
    static Response ReservedWord(const std::string& value);

    static Response ItemNameRegexBad(const std::string& r, const std::string& err);
    static Response SheetNameRegexBad(const std::string& r, const std::string& err);
    static Response ItemNameRegex(const std::string& r);
    static Response SheetNameRegex(const std::string& r);
};

}   // namespace Graph
