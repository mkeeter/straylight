#pragma once

#include <QAbstractItemModel>
#include <QString>

#include "graph/types/cell.hpp"
#include "graph/response.hpp"
//#include "app/bridge/iomodel.hpp"

namespace App {
namespace Bridge {

/*
 *  Generic item (can be used for either cell or instance)
 */
struct Item
{
    enum { CELL, INSTANCE } type;
    QString name;

    bool cell_valid;
    QString cell_expr;
    QString cell_value;
    bool cell_input;

    //IOModel instance_io; TODO
};

class ItemsModel : public QAbstractItemModel
{
public:
    ItemsModel(QObject* parent=0) : QAbstractItemModel(parent) {}

    void addItem(const Item& item);
    int rowCount(const QModelIndex& parent=QModelIndex()) const;

    /*
     *  Update the model from the given response
     */
    void operator()(const Graph::Response& r);

    QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const;

protected:
    enum ItemRole {
        TypeRole = Qt::UserRole + 1,
        NameRole,
        ValidRole,
        ExprRole,
        ValueRole,
        IsInputRole,
    };

    QHash<int, QByteArray> roleNames() const;

    QList<Item> items;
};

}   // namespace Bridge
}   // namespace App
