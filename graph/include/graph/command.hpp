#pragma once

#include <string>

#include "graph/types/index.hpp"
#include "graph/types/env.hpp"

namespace Graph {

class Root; // Forward declaration

struct Command
{
    Command() : op(INVALID), parent(0), target(0), self(0) {}

    enum {
        INVALID,

        RENAME_SHEET,
        INSERT_SHEET,
        ERASE_SHEET,

        RENAME_ITEM,

        INSERT_CELL,
        SET_EXPR,
        SET_INPUT,
        SET_EXPR_OR_INPUT,
        ERASE_CELL,

        INSERT_INSTANCE,
        ERASE_INSTANCE,

        CLEAR,

        PUSH_MACRO,
        POP_MACRO,

        UNDO,
        REDO,
        RESET_UNDO_QUEUE,
        UNDO_READY,
        REDO_READY,
        UNDO_NOT_READY,
        REDO_NOT_READY,

        VALUE_CHANGED,
        RESULT_CHANGED,

        ITEM_NAME_REGEX,
        SHEET_NAME_REGEX,
        RESERVED_WORD,
    } op;

    std::string name;
    std::string expr;

    ItemIndex parent;
    ItemIndex target;
    ItemIndex self;

    Env env;

    void operator()(Graph::Root& root);
};

}   // namespace Graph
