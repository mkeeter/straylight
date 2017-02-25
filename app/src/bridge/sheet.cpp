#include <cassert>

#include "app/bridge/sheet.hpp"
#include "app/bridge/graph.hpp"

namespace App {
namespace Bridge {

SheetModel::SheetModel(
        Graph::SheetIndex sheet, GraphModel* parent)
    : QObject(parent), graph(parent), sheet(sheet)
{
    // Nothing to do here
}

QObject* SheetModel::itemsModel()
{
    assert(current_env.size());
    items.setEnv(current_env);
    QQmlEngine::setObjectOwnership(&items, QQmlEngine::CppOwnership);
    return &items;
}

QObject* SheetModel::libraryModel()
{
    QQmlEngine::setObjectOwnership(&library, QQmlEngine::CppOwnership);
    return &library;
}

void SheetModel::insertCell()
{
    graph->enqueue(Graph::Command::InsertCell(
                sheet, items.nextItemName().toStdString(), ""));
}

QString SheetModel::checkItemRename(unsigned i, const QString& str)
{
    auto e = graph->isValidItemName(str);
    if (e.size())
    {
        return e;
    }

    return items.checkItemRename(i, str);
}

void SheetModel::setEnv(const Graph::Env& env)
{
    if (current_env != env)
    {
        current_env = env;
        // TODO
        //setInstanceName(QString::fromStdString(instance_names.at(env)));
    }
}

void SheetModel::renameItem(unsigned i, const QString& str)
{
    graph->enqueue(Graph::Command::RenameItem(i, str.toStdString()));
}

void SheetModel::setExpr(unsigned i, const QString& str)
{
    graph->enqueue(Graph::Command::SetExpr(i, str.toStdString()));
}

void SheetModel::eraseCell(unsigned i)
{
    graph->enqueue(Graph::Command::EraseCell(i));
}

void SheetModel::eraseInstance(unsigned i)
{
    graph->enqueue(Graph::Command::EraseInstance(i));
}

void SheetModel::insertSheet()
{
    graph->enqueue(Graph::Command::InsertSheet(
                sheet, library.nextSheetName().toStdString()));
}

void SheetModel::insertInstance(unsigned t)
{
    graph->enqueue(Graph::Command::InsertInstance(
                sheet, t, items.nextItemName().toStdString()));
}

void SheetModel::eraseSheet(unsigned t)
{
    graph->enqueue(Graph::Command::EraseSheet(t));
}

void SheetModel::renameSheet(unsigned sheet_index, const QString& str)
{
    graph->enqueue(Graph::Command::RenameSheet(
                sheet_index, str.toStdString()));
}

QString SheetModel::checkSheetRename(unsigned i, const QString& str) const
{
    auto e = graph->isValidSheetName(str);
    if (e.size())
    {
        return e;
    }

    return library.checkSheetRename(i, str);
}

void SheetModel::updateFrom(const Graph::Response& r)
{
    switch (r.op)
    {
        // Sheet library level operations
        case Graph::Response::SHEET_RENAMED:
        case Graph::Response::SHEET_ERASED:
        case Graph::Response::SHEET_INSERTED:
        case Graph::Response::SHEET_AVAILABLE:
        case Graph::Response::SHEET_UNAVAILABLE:
            library.updateFrom(r);
            break;

        // Item-level operations
        case Graph::Response::INSTANCE_RENAMED:
        case Graph::Response::CELL_RENAMED:
        case Graph::Response::CELL_INSERTED:
        case Graph::Response::CELL_ERASED:
        case Graph::Response::INSTANCE_ERASED:
        case Graph::Response::EXPR_CHANGED:
        case Graph::Response::INSTANCE_INSERTED:
        case Graph::Response::INSTANCE_SHEET_RENAMED:
        case Graph::Response::VALUE_CHANGED:
        case Graph::Response::RESULT_CHANGED:
        case Graph::Response::INPUT_CHANGED:
        case Graph::Response::INPUT_CREATED:
        case Graph::Response::OUTPUT_CREATED:
        case Graph::Response::IO_DELETED:
        case Graph::Response::CELL_TYPE_CHANGED:
            items.updateFrom(r);
            break;

        default:
            assert(false);
    }
}

}   // namespace Bridge
}   // namespace App
