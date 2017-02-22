#include <cassert>

#include "app/bridge/instance.hpp"
#include "app/bridge/graph.hpp"

namespace App {
namespace Bridge {

SheetInstanceModel::SheetInstanceModel(
        const Graph::Env& env, Graph::SheetIndex sheet,
        GraphModel* parent)
    : QObject(parent), graph(parent), env(env), sheet(sheet)
{
    // Nothing to do here
}

void SheetInstanceModel::insertCell()
{
    graph->enqueue(Graph::Command::InsertCell(
                sheet, items.nextItemName().toStdString(), ""));
}

QString SheetInstanceModel::checkItemRename(unsigned i, const QString& str)
{
    auto e = graph->isValidItemName(str);
    if (e.size())
    {
        return e;
    }

    return items.checkItemRename(i, str);
}

void SheetInstanceModel::renameItem(unsigned i, const QString& str)
{
    graph->enqueue(Graph::Command::RenameItem(i, str.toStdString()));
}

void SheetInstanceModel::setExpr(unsigned i, const QString& str)
{
    graph->enqueue(Graph::Command::SetExpr(i, str.toStdString()));
}

void SheetInstanceModel::eraseCell(unsigned i)
{
    graph->enqueue(Graph::Command::EraseCell(i));
}

void SheetInstanceModel::eraseInstance(unsigned i)
{
    graph->enqueue(Graph::Command::EraseInstance(i));
}

void SheetInstanceModel::insertSheet()
{
    graph->enqueue(Graph::Command::InsertSheet(
                sheet, library.nextSheetName().toStdString()));
}

void SheetInstanceModel::insertInstance(unsigned t)
{
    graph->enqueue(Graph::Command::InsertInstance(
                sheet, t, items.nextItemName().toStdString()));
}

void SheetInstanceModel::eraseSheet(unsigned t)
{
    graph->enqueue(Graph::Command::EraseSheet(t));
}

void SheetInstanceModel::renameSheet(unsigned sheet_index, const QString& str)
{
    graph->enqueue(Graph::Command::RenameSheet(
                sheet_index, str.toStdString()));
}

QString SheetInstanceModel::checkSheetRename(unsigned i, const QString& str) const
{
    auto e = graph->isValidSheetName(str);
    if (e.size())
    {
        return e;
    }

    return library.checkSheetRename(i, str);
}

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
