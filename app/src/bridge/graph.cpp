#include <cassert>

#include "app/bridge/graph.hpp"
#include "app/bind/bind_s7.hpp"
#include "kernel/bind/bind_s7.h"

namespace App {
namespace Bridge {

GraphModel::GraphModel(QObject* parent)
    : QObject(parent), responses(root.run(commands)), watcher(responses)
{
    // Inject the kernel bindings into the interpreter
    // TODO: this is unsafe, as the root is already running
    root.call(Kernel::Bind::init);
    root.call(App::Bind::init);

    connect(&watcher, &QueueWatcher::gotResponse,
            this, &GraphModel::gotResponse);
}

void GraphModel::updateFrom(const Graph::Response& r)
{
    qDebug() << "Dispatching" << r.op;
    switch (r.op)
    {
        // Sheet library level operations
        case Graph::Response::SHEET_RENAMED:
        case Graph::Response::SHEET_ERASED:
        case Graph::Response::SHEET_INSERTED:
        // Item-level operations
        case Graph::Response::ITEM_RENAMED:
        case Graph::Response::CELL_INSERTED:
        case Graph::Response::ITEM_ERASED:
        case Graph::Response::EXPR_CHANGED:
        case Graph::Response::INSTANCE_INSERTED:
        case Graph::Response::VALUE_CHANGED:
        case Graph::Response::RESULT_CHANGED:
            instances.at(r.env)->updateFrom(r);
            break;

        // Instance-level operations, which are execute on parent ItemModel
        case Graph::Response::INPUT_CHANGED:
        case Graph::Response::INPUT_CREATED:
        case Graph::Response::OUTPUT_CREATED:
        case Graph::Response::IO_DELETED:
        {
            auto env_ = r.env;
            env_.pop_back();
            instances.at(env_)->updateFrom(r);
            break;
        }

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

void GraphModel::gotResponse()
{
    updateFrom(responses.pop());
}

void GraphModel::setVariables(const Kernel::Solver::Solution& sol)
{
#if 0   // TODO
    {   // Roll out a set of variable updates
        auto lock = r.Lock();
        for (const auto& s : sol)
        {
            auto k = static_cast<Bind::CellKeyTag*>(
                    Kernel::Cache::instance()->tag(s.first))->key;

            std::stringstream ss;
            ss << s.second;
            r.setExprOrInput(k, ss.str());
        }
    }
    sync();
#endif
}


}   // namespace Bridge
}   // namespace App
