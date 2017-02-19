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

        // All the other operations have a special, non-executable meaning.
        case PUSH_MACRO:
        case POP_MACRO:
        case UNDO:
        case REDO:
        case RESET_UNDO_QUEUE:
        case UNDO_READY:
        case REDO_READY:
        case UNDO_NOT_READY:
        case REDO_NOT_READY:
        case VALUE_CHANGED:
        case RESULT_CHANGED:
        case ITEM_NAME_REGEX:
        case SHEET_NAME_REGEX:
        case RESERVED_WORD:
        case INVALID:
            assert(false);
    };
}

}   // namespace Graph
