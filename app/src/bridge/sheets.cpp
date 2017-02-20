#include <cassert>

#include "app/bridge/sheets.hpp"

namespace App {
namespace Bridge {

int SheetsModel::rowCount(const QModelIndex& parent) const
{
    (void)parent;
    return sheets.size();
}

QVariant SheetsModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= sheets.count())
    {
        return QVariant();
    }

    const Sheet &s = sheets[index.row()];
    switch (role) {
        case NameRole:  return s.name;
        case EditableRole:  return s.editable;
        case InsertableRole:  return s.insertable;
        default: return QVariant();
    }
}

QHash<int, QByteArray> SheetsModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {EditableRole, "editable"},
        {InsertableRole, "insertable"}};
}

void SheetsModel::updateFrom(const Graph::Response& r)
{
    switch (r.op)
    {
        case Graph::Response::SHEET_RENAMED:
        case Graph::Response::SHEET_ERASED:
        case Graph::Response::SHEET_INSERTED:
            assert(false); // TODO

        default:
            assert(false);
    }
}

}   // namespace Bridge
}   // namespace App

