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

        ITEM_RENAMED,
        CELL_INSERTED,
        CELL_TYPE_CHANGED,
        EXPR_CHANGED,
        INPUT_CHANGED,
        INSTANCE_INSERTED,
        ITEM_ERASED,

        VALUE_CHANGED,
        RESULT_CHANGED,
        INPUT_CREATED,
        OUTPUT_CREATED,
        IO_DELETED,

        ITEM_NAME_REGEX,
        SHEET_NAME_REGEX,
        RESERVED_WORD,
        RESET_UNDO_QUEUE,
        UNDO_READY,
        REDO_READY,
        UNDO_NOT_READY,
        REDO_NOT_READY,

        CLEAR,
        HALT,
    } op;

    // Environment for this change to be executed
    Env env;

    // Enough variables to handle all the possible operations
    std::string name;
    std::string expr;

    ItemIndex target;

    bool valid;
    Cell::Type type;

    //  Helper constructors
    static Response ItemErased(const Env& env, const ItemIndex& i);
    static Response IOErased(const CellKey& k);
    static Response CellInserted(
            const CellKey& k, const std::string& name,
            const std::string& expr);
    static Response InstanceInserted(
            const Env& env, const InstanceIndex& i,
            const std::string& name, const std::string& sheet_name);
    static Response InputCreated(
            const CellKey& k, const std::string& name, const std::string& expr);
    static Response OutputCreated(const CellKey& k, const std::string& name);
    static Response ExprChanged(const CellKey& c, const std::string& expr);
    static Response InputChanged(const CellKey& c, const std::string& expr);
    static Response ItemRenamed(
            const Env& env, const ItemIndex& i, const std::string& name);
    static Response SheetCreated(
            const Env& env, const SheetIndex& i, const std::string& name);
    static Response SheetRenamed(
            const Env& env, const SheetIndex& i, const std::string& name);
    static Response SheetErased(
            const Env& env, const SheetIndex& i);
    static Response ValueChanged(
            const CellKey& k, const std::string& value, bool valid);
    static Response CellTypeChanged(const CellKey& k, Cell::Type type);
    static Response Halt();
    static Response ReservedWord(const std::string& value);

    /* TODO
     *  InstanceSheetRenamed
     *  InputRenamed
     *  OutputRenamed
     */
};

}   // namespace Graph
