#include "graph/command.hpp"
#include "graph/root.hpp"

namespace Graph {

void Command::operator()(Graph::Root& root)
{
    switch (op)
    {
        case RENAME_SHEET:
            root.renameSheet(SheetIndex(target), name);
            break;

        case INSERT_SHEET:
            if (self != 0)
            {
                root.insertSheet(SheetIndex(parent), SheetIndex(self), name);
            }
            else
            {
                root.insertSheet(SheetIndex(parent), name);
            }
            break;

        case ERASE_SHEET:
            root.eraseSheet(SheetIndex(target));
            break;

        case RENAME_ITEM:
            root.renameItem(target, name);
            break;

        case INSERT_CELL:
            if (self != 0)
            {
                root.insertCell(SheetIndex(parent), CellIndex(self),
                                name, expr);
            }
            else
            {
                root.insertCell(SheetIndex(parent), name, expr);
            }
            break;

        case SET_EXPR:
            root.setExpr(CellIndex(target), expr);
            break;

        case SET_INPUT:
            root.setInput(InstanceIndex(parent), CellIndex(target), expr);
            break;

        case SET_EXPR_OR_INPUT:
            root.setExprOrInput({env, CellIndex(target)}, expr);
            break;

        case ERASE_CELL:
            root.eraseCell(CellIndex(target));
            break;

        case INSERT_INSTANCE:
            if (self != 0)
            {
                root.insertInstance(SheetIndex(parent), InstanceIndex(self),
                                    name, SheetIndex(target));
            }
            else
            {
                root.insertInstance(SheetIndex(parent), name, SheetIndex(target));
            }
            break;

        case ERASE_INSTANCE:
            root.eraseInstance(InstanceIndex(target));
            break;

        case CLEAR:
            root.clear();
            break;

        case UNDO:
        case REDO:
            // TODO
            assert(false);

        // All the other operations have a special, non-executable meaning.
        case PUSH_MACRO:
        case POP_MACRO:
        case INVALID:
        case STOP_LOOP:
            assert(false);
    };
}

Command Command::StopLoop()
{
    return { STOP_LOOP, "", "", 0, 0, 0, {}};
}

Command Command::InsertCell(
        SheetIndex s, const std::string& name, const std::string& expr)
{
    return { INSERT_CELL, name, expr, 0, s, 0, {}};
}

}   // namespace Graph
