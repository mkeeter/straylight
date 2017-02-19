#include "app/bridge/items.hpp"

namespace App {
namespace Bridge {

void ItemsModel::addItem(const Item& item)
{
    items.push_back(item);
}

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
        case IsInputRole:  return i.type == Item::CELL ? i.cell_input : QVariant();
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

}   // namespace Bridge
}   // namespace App
