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
        case IOTypeRole: return i.type == Item::CELL ? i.cell_type : QVariant();
        case UniqueIndexRole: return i.unique_index;
        default: return QVariant();
    }
}

QString ItemsModel::nextItemName(QString prefix) const
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

QString ItemsModel::checkItemRename(int i, const QString& str) const
{
    auto s = str.toStdString();

    auto f = names.right.find(s);
    return (f != names.right.end() && f->second != i)
        ? "Name already in use"
        : "";
}

QHash<int, QByteArray> ItemsModel::roleNames() const
{
    return {
        {TypeRole, "type"},
        {NameRole, "name"},
        {ValidRole, "valid"},
        {ExprRole, "expr"},
        {ValueRole, "value"},
        {IOTypeRole, "ioType"},
        {UniqueIndexRole, "uniqueIndex"}};
}

void ItemsModel::updateFrom(const Graph::Response& r)
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
                names.left.replace_data(names.left.find(r.target), r.name);
                dataChanged(i, i, {NameRole});
            }
            break;
        }

        case Graph::Response::EXPR_CHANGED:
        {
            auto index = order.at(r.target);
            auto new_expr = QString::fromStdString(r.expr);
            auto& item = items[index];
            if (item.cell_expr != new_expr)
            {
                item.cell_expr = new_expr;
                auto i = createIndex(index, 0);
                dataChanged(i, i, {ExprRole});
            }
            break;
        }

        case Graph::Response::VALUE_CHANGED:
        {
            auto index = order.at(r.target);
            auto new_value = QString::fromStdString(r.expr);
            auto& item = items[index];
            bool new_valid = r.flags & Graph::Response::RESPONSE_FLAG_VALID;
            if (item.cell_value != new_value || item.cell_valid != new_valid)
            {
                items[index].cell_value = new_value;
                items[index].cell_valid = new_valid;
                auto i = createIndex(index, 0);
                dataChanged(i, i, {ValueRole, ValidRole});
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
                items.push_back(Item::Cell(
                            Graph::CellIndex(r.target), r.name, r.expr));
                order.insert({r.target, index});
                names.left.insert({r.target, r.name});
            endInsertRows();
            break;
        }

        case Graph::Response::INSTANCE_INSERTED:
        {
            auto index = items.size();
            beginInsertRows(QModelIndex(), index, index);
                items.push_back(Item::Instance(
                            Graph::InstanceIndex(r.target), r.name, r.expr));
                order.insert({r.target, index});
                names.left.insert({r.target, r.name});
            endInsertRows();
            break;
        }
        case Graph::Response::ITEM_ERASED:
        {
            auto index = order.at(r.target);
            beginRemoveRows(QModelIndex(), index, index);
                items.removeAt(index);
                order.erase(r.target);
                names.left.erase(r.target);
                // Shift later items down by one
                // TODO: replace this with an ORDER_CHANGED Response
                for (const auto& i : items)
                {
                    if (order.at(i.unique_index) >= index)
                    {
                        order.at(i.unique_index)--;
                    }
                }
            endRemoveRows();
            break;
        }

        case Graph::Response::CELL_TYPE_CHANGED:
        {
            auto index = order.at(r.target);

            QString t;
            if      (r.flags & Graph::Response::RESPONSE_FLAG_TYPE_BASE)
                {t = "basic";}
            else if (r.flags & Graph::Response::RESPONSE_FLAG_TYPE_INPUT)
                {t = "input";}
            else if (r.flags & Graph::Response::RESPONSE_FLAG_TYPE_OUTPUT)
                {t = "output";}
            else if (r.flags & Graph::Response::RESPONSE_FLAG_TYPE_UNKNOWN)
                {t = "unknown";}

            auto& item = items[index];
            if (item.cell_type != t)
            {
                item.cell_type = t;
                auto i = createIndex(index, 0);
                dataChanged(i, i, {IOTypeRole});
            }
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
