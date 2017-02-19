#include <cassert>

#include "app/bridge/items.hpp"

namespace App {
namespace Bridge {

int ItemsModel::rowCount(const QModelIndex& parent) const
{
    (void)parent;
    return items.size();
}

QVariant ItemsModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= items.count())
    {
        return QVariant();
    }

    const Item &i = items[index.row()];
    switch (role) {
        case TypeRole:  return i.type == Item::CELL ? "cell" : "instance";
        case NameRole:  return i.name;
        case ValidRole: return i.type == Item::CELL ? i.cell_valid : QVariant();
        case ExprRole:  return i.type == Item::CELL ? i.cell_expr : QVariant();
        case ValueRole:  return i.type == Item::CELL ? i.cell_value : QVariant();
        default: return QVariant();
    }
}

QHash<int, QByteArray> ItemsModel::roleNames() const
{
    return {
        {TypeRole, "type"},
        {NameRole, "name"},
        {ValidRole, "valid"},
        {ExprRole, "expr"},
        {ValueRole, "valid"}};
}

void ItemsModel::operator()(const Graph::Response& r)
{
    switch (r.op)
    {
        case Graph::Response::ITEM_RENAMED:
        {
            auto index = order.at(r.target);
            auto new_name = QString::fromStdString(r.name);
            if (items[index].name != new_name)
            {
                items[index].name = new_name;
                auto i = createIndex(index, 0);
                dataChanged(i, i, {NameRole});
            }
            break;
        }

        case Graph::Response::EXPR_CHANGED:
        {
            auto index = order.at(r.target);
            auto new_expr = QString::fromStdString(r.expr);
            if (items[index].cell_expr != new_expr)
            {
                items[index].cell_expr = new_expr;
                auto i = createIndex(index, 0);
                dataChanged(i, i, {ExprRole});
            }
            break;
        }

        case Graph::Response::VALUE_CHANGED:
        {
            auto index = order.at(r.target);
            auto new_value = QString::fromStdString(r.expr);
            if (items[index].cell_value != new_value)
            {
                items[index].cell_value = new_value;
                auto i = createIndex(index, 0);
                dataChanged(i, i, {ValueRole});
            }
            break;
        }
        case Graph::Response::RESULT_CHANGED:
            assert(false); // TODO
            break;

        case Graph::Response::CELL_INSERTED:
        {
            auto index = items.size();
            beginInsertRows(QModelIndex(), index, index);
            items.push_back(Item::Cell(r.name, r.expr));
            endInsertRows();
            break;
        }

        case Graph::Response::INSTANCE_INSERTED:
        {
            auto index = items.size();
            beginInsertRows(QModelIndex(), index, index);
            items.push_back(Item::Instance(r.name, r.expr));
            endInsertRows();
            break;
        }
        case Graph::Response::ITEM_ERASED:
        {
            auto index = order.at(r.target);
            beginRemoveRows(QModelIndex(), index, index);
            items.removeAt(index);
            endRemoveRows();
            order.erase(r.target);
            break;
        }


        case Graph::Response::INPUT_CHANGED:
        case Graph::Response::INPUT_CREATED:
        case Graph::Response::OUTPUT_CREATED:
        case Graph::Response::IO_DELETED:
            assert(false); // TODO

        default:
            assert(false);
    }
}

}   // namespace Bridge
}   // namespace App
