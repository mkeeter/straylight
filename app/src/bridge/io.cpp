#include <cassert>

#include "app/bridge/io.hpp"

namespace App {
namespace Bridge {

int IOModel::rowCount(const QModelIndex& parent) const
{
    (void)parent;
    return cells.size();
}

QVariant IOModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= cells.count())
    {
        return QVariant();
    }

    const IOCell &i = cells[index.row()];
    switch (role) {
        case TypeRole:  return i.type == IOCell::INPUT ? "input" : "output";
        case NameRole:  return i.name;
        case ValidRole: return i.valid.count(env) ? i.valid.at(env) : false;
        case ExprRole:  return i.expr;
        case ValueRole: return i.value.count(env) ? i.value.at(env) : "";
        case UniqueIndexRole:   return i.unique_index;
        default: return QVariant();
    }
}

QHash<int, QByteArray> IOModel::roleNames() const
{
    return {
        {TypeRole, "type"},
        {NameRole, "name"},
        {ValidRole, "valid"},
        {ExprRole, "expr"},
        {ValueRole, "value"},
        {UniqueIndexRole, "uniqueIndex"}};
}

void IOModel::updateFrom(const Graph::Response& r)
{
    switch (r.op)
    {
        case Graph::Response::IO_INPUT_CREATED:
        {
            auto index = cells.size();
            beginInsertRows(QModelIndex(), index, index);
                cells.push_back(IOCell {
                        IOCell::INPUT,
                        r.other.i,
                        QString::fromStdString(r.name),
                        std::map<Graph::Env, bool>(),
                        std::map<Graph::Env, QString>(),
                        QString::fromStdString(r.expr) });
                order.insert({Graph::CellIndex(r.other), index});
                emit(countChanged());
            endInsertRows();
            break;
        }

        case Graph::Response::IO_OUTPUT_CREATED:
        {
            auto index = cells.size();
            beginInsertRows(QModelIndex(), index, index);
                cells.push_back(IOCell {
                        IOCell::OUTPUT,
                        r.other.i,
                        QString::fromStdString(r.name),
                        std::map<Graph::Env, bool>(),
                        std::map<Graph::Env, QString>(),
                        QString::fromStdString(r.expr) });
                order.insert({Graph::CellIndex(r.other), index});
                emit(countChanged());
            endInsertRows();
            break;
        }

        case Graph::Response::IO_RENAMED:
        {
            auto index = order.at(Graph::CellIndex(r.other));
            auto new_name = QString::fromStdString(r.name);
            if (cells[index].name != new_name)
            {
                cells[index].name = new_name;
                auto i = createIndex(index, 0);
                dataChanged(i, i, {NameRole});
            }
            break;
        }

        case Graph::Response::IO_DELETED:
        {
            auto index = order.at(Graph::CellIndex(r.other));
            beginRemoveRows(QModelIndex(), index, index);
                cells.removeAt(index);
                order.erase(Graph::CellIndex(r.other));

                // Shift later items down by one
                // TODO: replace this with an IO_ORDER_CHANGED Response?
                for (const auto& i : cells)
                {
                    if (order.at(i.unique_index) >= index)
                    {
                        order.at(i.unique_index)--;
                    }
                }
                emit(countChanged());
            endRemoveRows();
            break;
        }

        case Graph::Response::IO_VALUE_CHANGED:
        {
            auto index = order.at(Graph::CellIndex(r.other));
            auto new_value = QString::fromStdString(r.expr);
            auto& cell = cells[index];
            bool new_valid = r.flags & Graph::Response::RESPONSE_FLAG_VALID;
            if (cell.value.count(r.env) == 0 ||
                cell.value.at(r.env) != new_value ||
                cell.valid.count(r.env) == 0 ||
                cell.valid.at(r.env) != new_valid)
            {
                cell.value[r.env] = new_value;
                cell.valid[r.env] = new_valid;
                auto i = createIndex(index, 0);
                dataChanged(i, i, {ValueRole, ValidRole});
            }
            break;
        }

        case Graph::Response::IO_EXPR_CHANGED:
        {
            auto index = order.at(Graph::CellIndex(r.other));
            auto new_expr = QString::fromStdString(r.expr);
            auto& cell = cells[index];
            if (cell.expr != new_expr)
            {
                cell.expr = new_expr;
                auto i = createIndex(index, 0);
                dataChanged(i, i, {ExprRole});
            }
            break;
        }

        default:
            assert(false);
    }
}

}   // namespace Bridge
}   // namespace App

