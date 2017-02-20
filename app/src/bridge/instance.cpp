#include <cassert>

#include "app/bridge/instance.hpp"

namespace App {
namespace Bridge {

void SheetInstanceModel::updateFrom(const Graph::Response& r)
{
    switch (r.op)
    {
        // Sheet library level operations
        case Graph::Response::SHEET_RENAMED:
        case Graph::Response::SHEET_ERASED:
        case Graph::Response::SHEET_INSERTED:
            library.updateFrom(r);
            break;

        // Item-level operations
        case Graph::Response::ITEM_RENAMED:
        case Graph::Response::CELL_INSERTED:
        case Graph::Response::ITEM_ERASED:
        case Graph::Response::EXPR_CHANGED:
        case Graph::Response::INSTANCE_INSERTED:
        case Graph::Response::VALUE_CHANGED:
        case Graph::Response::RESULT_CHANGED:
        case Graph::Response::INPUT_CHANGED:
        case Graph::Response::INPUT_CREATED:
        case Graph::Response::OUTPUT_CREATED:
        case Graph::Response::IO_DELETED:
            items.updateFrom(r);
            break;

        case Graph::Response::ITEM_NAME_REGEX:
        case Graph::Response::SHEET_NAME_REGEX:
        case Graph::Response::RESERVED_WORD:
        case Graph::Response::RESET_UNDO_QUEUE:
        case Graph::Response::UNDO_READY:
        case Graph::Response::REDO_READY:
        case Graph::Response::UNDO_NOT_READY:
        case Graph::Response::REDO_NOT_READY:
            // TODO

        case Graph::Response::CLEAR:
            assert(false);

        case Graph::Response::HALT:
            assert(false);
    }
}

}   // namespace Bridge
}   // namespace App