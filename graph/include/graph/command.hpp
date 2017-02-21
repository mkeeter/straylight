#pragma once

#include <string>

#include "graph/types/index.hpp"
#include "graph/types/keys.hpp"
#include "graph/types/env.hpp"

namespace Graph {

class Root; // Forward declaration

struct Command
{
    enum {
        RENAME_SHEET,
        INSERT_SHEET,
        ERASE_SHEET,

        RENAME_ITEM,

        INSERT_CELL,
        SET_EXPR,
        SET_INPUT,
        ERASE_CELL,

        INSERT_INSTANCE,
        ERASE_INSTANCE,

        SET_EXPR_OR_INPUT,
        UNDO,
        REDO,

        CLEAR,

        //  Meta commands
        PUSH_MACRO,
        POP_MACRO,

        INVALID,
    } op;

    // Enough variables to handle all the possible operations
    std::string name;
    std::string expr;

    ItemIndex parent;
    ItemIndex target;
    ItemIndex self;

    Env env;

    void operator()(Graph::Root& root);

    static Command InsertCell(
            SheetIndex s, const std::string& name, const std::string& expr);
    static Command RenameItem(ItemIndex i, const std::string& name);
    static Command SetExpr(CellIndex i, const std::string& expr);
};

}   // namespace Graph
