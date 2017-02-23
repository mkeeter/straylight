#pragma once

#include <string>

#include "graph/types/index.hpp"
#include "graph/types/keys.hpp"
#include "graph/types/env.hpp"
#include "graph/types/cell.hpp"

namespace Graph {

struct Response
{
    enum {
        SHEET_RENAMED,
        SHEET_INSERTED,
        SHEET_ERASED,

        CELL_RENAMED,
        INSTANCE_RENAMED,
        INSTANCE_SHEET_RENAMED,
        CELL_INSERTED,
        CELL_TYPE_CHANGED,
        EXPR_CHANGED,
        INPUT_CHANGED,
        INSTANCE_INSERTED,
        CELL_ERASED,
        INSTANCE_ERASED,

        VALUE_CHANGED,
        RESULT_CHANGED,
        INPUT_CREATED,
        OUTPUT_CREATED,
        IO_DELETED,

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

        CLEAR,
    } op;

    // Environment for this change to be executed
    Env env;

    // Enough variables to handle all the possible operations
    std::string name;
    std::string expr;

    ItemIndex target;
    SheetIndex sheet;

    enum ResponseFlag {
        RESPONSE_FLAG_VALID         = (1 << 0),
        RESPONSE_FLAG_EDITABLE      = (1 << 1),
        RESPONSE_FLAG_INSERTABLE    = (1 << 2),
        RESPONSE_FLAG_TYPE_INPUT    = (1 << 3),
        RESPONSE_FLAG_TYPE_OUTPUT   = (1 << 4),
        RESPONSE_FLAG_TYPE_BASE     = (1 << 5),
        RESPONSE_FLAG_TYPE_UNKNOWN  = (1 << 6),
    };
    uint8_t flags;

    //  Helper constructors
    static Response CellErased(const Env& env, const CellIndex& i);
    static Response InstanceErased(const Env& env, const InstanceIndex& i);
    static Response IOErased(const CellKey& k);
    static Response CellInserted(
            const CellKey& k, const std::string& name,
            const std::string& expr);
    static Response InstanceInserted(
            const Env& env, const InstanceIndex& i, const SheetIndex& target,
            const std::string& name, const std::string& sheet_name);
    static Response InputCreated(
            const CellKey& k, const std::string& name, const std::string& expr);
    static Response OutputCreated(const CellKey& k, const std::string& name);
    static Response ExprChanged(const CellKey& c, const std::string& expr);
    static Response InputChanged(const CellKey& c, const std::string& expr);
    static Response CellRenamed(
            const Env& env, const CellIndex& i, const std::string& name);
    static Response InstanceRenamed(
            const Env& env, const InstanceIndex& i, const std::string& name);
    static Response SheetCreated(
            const Env& env, const SheetIndex& i, const std::string& name);
    static Response SheetRenamed(
            const Env& env, const SheetIndex& i, const std::string& name);
    static Response SheetErased(
            const Env& env, const SheetIndex& i);
    static Response ValueChanged(
            const CellKey& k, const std::string& value, bool valid);
    static Response CellTypeChanged(const CellKey& k, Cell::Type type);
    static Response ReservedWord(const std::string& value);

    static Response ItemNameRegexBad(const std::string& r, const std::string& err);
    static Response SheetNameRegexBad(const std::string& r, const std::string& err);
    static Response ItemNameRegex(const std::string& r);
    static Response SheetNameRegex(const std::string& r);
    static Response InstanceSheetRenamed(
        const Env& env, const InstanceIndex& i, const std::string& sheet_name);

    /* TODO
     *  InstanceSheetRenamed
     *  InputRenamed
     *  OutputRenamed
     */
};

}   // namespace Graph
