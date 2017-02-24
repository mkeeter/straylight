#include <cassert>

#include "app/bridge/library.hpp"

namespace App {
namespace Bridge {

int LibraryModel::rowCount(const QModelIndex& parent) const
{
    (void)parent;
    return sheets.size();
}

QVariant LibraryModel::data(const QModelIndex& index, int role) const
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
        case SheetIndexRole:    return s.sheet_index.i;
        default: return QVariant();
    }
}

QHash<int, QByteArray> LibraryModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {EditableRole, "editable"},
        {InsertableRole, "insertable"},
        {SheetIndexRole, "sheetIndex"}};
}

QString LibraryModel::nextSheetName(QString prefix) const
{
    std::string base = prefix.toStdString();
    for (int i=0; true; ++i)
    {
        std::stringstream ss;
        ss << base;
        ss << i;

        auto str = ss.str();

        if (names.right.find(str) == names.right.end())
        {
            return QString::fromStdString(str);
        }
    }
}

QString LibraryModel::checkSheetRename(int i, const QString& str) const
{
    auto s = str.toStdString();

    auto f = names.right.find(s);
    return (f != names.right.end() && f->second != i)
        ? "Name already in use"
        : "";
}

void LibraryModel::updateFrom(const Graph::Response& r)
{
    auto target = Graph::SheetIndex(r.target);
    switch (r.op)
    {
        case Graph::Response::SHEET_RENAMED:
        {
            auto index = order.at(target);
            auto new_name = QString::fromStdString(r.name);
            if (sheets[index].name != new_name)
            {
                sheets[index].name = new_name;
                auto i = createIndex(index, 0);
                names.left.replace_data(names.left.find(target), r.name);
                dataChanged(i, i, {NameRole});
            }
            break;
        }

        case Graph::Response::SHEET_ERASED:
        {
            auto index = order.at(target);
            beginRemoveRows(QModelIndex(), index, index);
                sheets.removeAt(index);
                order.erase(target);
                names.left.erase(target);
                // Shift later items down by one
                // TODO: replace this with an SHEET_ORDER_CHANGED Response
                for (const auto& i : sheets)
                {
                    if (order.at(i.sheet_index) >= index)
                    {
                        order.at(i.sheet_index)--;
                    }
                }
                emit(countChanged());
            endRemoveRows();
            break;
        }

        case Graph::Response::SHEET_INSERTED:
        {
            auto index = sheets.size();
            beginInsertRows(QModelIndex(), index, index);
                sheets.push_back(Sheet {
                        QString::fromStdString(r.name),
                        bool(r.flags & Graph::Response::RESPONSE_FLAG_EDITABLE),
                        bool(r.flags & Graph::Response::RESPONSE_FLAG_INSERTABLE),
                        target });
                order.insert({target, index});
                names.left.insert({target, r.name});
                emit(countChanged());
            endInsertRows();
            break;
        }

        default:
            assert(false);
    }
}

}   // namespace Bridge
}   // namespace App

