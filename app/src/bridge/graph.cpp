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
    // Add a model for the root sheet
    sheets[Graph::Tree::ROOT_SHEET].reset(
            new SheetModel(Graph::Tree::ROOT_SHEET, this));
    instances.insert({Graph::Tree::ROOT_INSTANCE, Graph::Tree::ROOT_SHEET});

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

void GraphModel::clear()
{
    commands.push(Graph::Command::Clear());
}

void GraphModel::updateFrom(const Graph::Response& r)
{
    switch (r.op)
    {
        ////////////////////////////////////////////////////////////////////////
        // Sheet operations also manipulate the graph sheet list
        case Graph::Response::SHEET_RENAMED:
        {
            sheets.at(Graph::SheetIndex(r.target))->setSheetName(
                    QString::fromStdString(r.name));
            sheets.at(r.sheet)->updateFrom(r);
            break;
        }
        case Graph::Response::SHEET_ERASED:
        {
            sheets.at(r.sheet)->updateFrom(r);
            sheets.erase(Graph::SheetIndex(r.target));
            break;
        }
        case Graph::Response::SHEET_INSERTED:
        {
            // Add a new sheet to the sheets list
            auto s = Graph::SheetIndex(r.target);
            sheets[s].reset(new SheetModel(s, this));
            sheets[s]->setSheetName(QString::fromStdString(r.name));
            sheets.at(r.sheet)->updateFrom(r);
            break;
        }
        ////////////////////////////////////////////////////////////////////////
        // Item-level operations
        case Graph::Response::CELL_INSERTED:
        case Graph::Response::CELL_ERASED:
        case Graph::Response::EXPR_CHANGED:
        case Graph::Response::VALUE_CHANGED:
        case Graph::Response::RESULT_CHANGED:
        case Graph::Response::CELL_TYPE_CHANGED:
        case Graph::Response::CELL_RENAMED:
        case Graph::Response::IO_EXPR_CHANGED:
        case Graph::Response::IO_VALUE_CHANGED:
        case Graph::Response::IO_INPUT_CREATED:
        case Graph::Response::IO_OUTPUT_CREATED:
        case Graph::Response::IO_DELETED:
        case Graph::Response::IO_RENAMED:
        case Graph::Response::SHEET_AVAILABLE:
        case Graph::Response::SHEET_UNAVAILABLE:
            sheets.at(r.sheet)->updateFrom(r);
            break;

        ////////////////////////////////////////////////////////////////////////
        // Instance creation both manipulates the instance map
        // and modifies the parent sheet
        case Graph::Response::INSTANCE_INSERTED:
        {
            // Update the parent sheet containing this particular instance
            sheets.at(r.sheet)->updateFrom(r);

            // Then update the map to add a new SheetInstanceModel
            Graph::InstanceIndex instance(r.target);
            Graph::SheetIndex sheet(r.other);

            auto s = sheets.at(sheet).get();
            s->setInstanceName(instance, r.name);
            s->setSheetName(QString::fromStdString(r.expr));

            instances.insert({instance, sheet});
            break;
        }

        case Graph::Response::INSTANCE_SHEET_RENAMED:
        {
            sheets.at(r.sheet)->updateFrom(r);
            break;
        }
        case Graph::Response::INSTANCE_ERASED:
        {
            emit(instanceErased(r.target.i));
            instances.erase(Graph::InstanceIndex(r.target));
            sheets.at(r.sheet)->updateFrom(r);
            break;
        }
        case Graph::Response::INSTANCE_RENAMED:
        {
            sheets.at(r.sheet)->updateFrom(r);
            auto i = Graph::InstanceIndex(r.target);
            auto s = instances.at(i);
            sheets.at(s)->setInstanceName(i, r.name);

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
    auto i = sheets.at(instances.at(env_.back())).get();
    QQmlEngine::setObjectOwnership(i, QQmlEngine::CppOwnership);
    i->setEnv(env_);
    return i;
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
