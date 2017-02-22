#include <cassert>

#include "app/bridge/graph.hpp"
#include "app/bridge/bridge.hpp"

#include "app/bind/bind_s7.hpp"
#include "kernel/bind/bind_s7.h"

namespace App {
namespace Bridge {

GraphModel::GraphModel(QObject* parent)
    : QObject(parent), responses(runRoot(root, commands)), watcher(responses)
{
    // Add a model for the root instance
    instances[{}].reset(new SheetInstanceModel({}, 0, this));

    // The first response pushed should create the root instance
    while (responses.empty())
    {
        responses.wait();
    }
    auto r = responses.pop();
    assert(r.op == Graph::Response::INSTANCE_INSERTED);
    assert(r.env.size() == 0);
    updateFrom(r);

    connect(&watcher, &QueueWatcher::gotResponse,
            this, &GraphModel::gotResponse);
    watcher.start();
}

GraphModel::~GraphModel()
{
    commands.halt();
    root.wait();
    watcher.wait();
}

shared_queue<Graph::Response>& GraphModel::runRoot(Graph::AsyncRoot& root,
        shared_queue<Graph::Command>& commands)
{
    root.call(Kernel::Bind::init);
    root.call(App::Bind::init);

    return root.run(commands);
}

QString GraphModel::isValidItemName(QString s) const
{
    for (auto& r : bad_item_names)
    {
        if (r.first.match(s).hasMatch())
        {
            return r.second;
        }
    }
    if (!good_item_name.match(s).hasMatch())
    {
        return "Invalid name";
    }
    return keywords.contains(s) ? "Interpreter keyword" : "";
}

QString GraphModel::isValidSheetName(QString s) const
{
    for (auto& r : bad_sheet_names)
    {
        if (r.first.match(s).hasMatch())
        {
            return r.second;
        }
    }
    if (!good_sheet_name.match(s).hasMatch())
    {
        return "Invalid name";
    }
    return keywords.contains(s) ? "Interpreter keyword" : "";
}

void GraphModel::updateFrom(const Graph::Response& r)
{
    qDebug() << "Dispatching" << r.op;
    switch (r.op)
    {
        // Instance creation both manipulates the instance map
        // and modifies the parent sheet
        case Graph::Response::INSTANCE_INSERTED:
        {
            // Update the parent sheet containing this particular instance
            instances.at(r.env)->updateFrom(r);

            // Then update the map to add a new SheetInstanceModel
            auto e = r.env;
            e.push_back(Graph::InstanceIndex(r.target));

            auto i = new SheetInstanceModel(e, r.sheet, this);
            instances[e].reset(i);
            i->setInstanceName(QString::fromStdString(r.name));
            i->setSheetName(QString::fromStdString(r.expr));

            break;
        }

        case Graph::Response::INSTANCE_SHEET_RENAMED:
        {
            instances.at(r.env)->updateFrom(r);

            auto e = r.env;
            e.push_back(Graph::InstanceIndex(r.target));
            auto i = instances.at(e).get();
            i->setSheetName(QString::fromStdString(r.expr));

            break;
        }

        // Sheet library level operations
        case Graph::Response::SHEET_RENAMED:
        case Graph::Response::SHEET_ERASED:
        case Graph::Response::SHEET_INSERTED:
        // Item-level operations
        case Graph::Response::CELL_INSERTED:
        case Graph::Response::ITEM_ERASED:
        case Graph::Response::EXPR_CHANGED:
        case Graph::Response::VALUE_CHANGED:
        case Graph::Response::RESULT_CHANGED:
        case Graph::Response::CELL_TYPE_CHANGED:
            instances.at(r.env)->updateFrom(r);
            break;

        case Graph::Response::ITEM_RENAMED:
        {
            auto e = r.env;
            e.push_back(Graph::InstanceIndex(r.target));

            auto i = instances.at(e).get();
            i->setInstanceName(QString::fromStdString(r.name));

            instances.at(r.env)->updateFrom(r);
            break;
        }

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

////////////////////////////////////////////////////////////////////////////////

        // Load reserved words from interpreter
        case Graph::Response::RESERVED_WORD:
            keywords.insert(QString::fromStdString(r.expr));
            break;

        // Load regular expressions in regex matchers
        case Graph::Response::ITEM_NAME_REGEX_BAD:
            bad_item_names.push_back({
                    QRegularExpression(QString::fromStdString(r.expr)),
                    QString::fromStdString(r.name)});
            break;
        case Graph::Response::SHEET_NAME_REGEX_BAD:
            bad_sheet_names.push_back({
                    QRegularExpression(QString::fromStdString(r.expr)),
                    QString::fromStdString(r.name)});
            break;
        case Graph::Response::ITEM_NAME_REGEX_GOOD:
            good_item_name = QRegularExpression(QString::fromStdString(r.expr));
            break;
        case Graph::Response::SHEET_NAME_REGEX_GOOD:
            good_sheet_name = QRegularExpression(QString::fromStdString(r.expr));
            break;

////////////////////////////////////////////////////////////////////////////////

        case Graph::Response::RESET_UNDO_QUEUE:
        case Graph::Response::UNDO_READY:
        case Graph::Response::REDO_READY:
        case Graph::Response::UNDO_NOT_READY:
        case Graph::Response::REDO_NOT_READY:
            // TODO

        case Graph::Response::CLEAR:
            assert(false);
    }
}

void GraphModel::gotResponse()
{
    if (!responses.empty())
    {
        auto r = responses.pop();
        updateFrom(r);
    }
}

QObject* GraphModel::modelOf(QList<int> env)
{
    Graph::Env env_;
    for (auto e : env)
    {
        env_.push_back(e);
    }
    return instances.at(env_).get();
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

////////////////////////////////////////////////////////////////////////////////

QObject* GraphModel::instance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    (void)engine;
    (void)scriptEngine;
    return instance();
}

GraphModel* GraphModel::instance()
{
    return Bridge::instance()->graph();
}

}   // namespace Bridge
}   // namespace App
