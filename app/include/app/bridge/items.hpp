#pragma once

#include <QAbstractListModel>
#include <QString>
#include <QSet>

#include <boost/bimap.hpp>

#include "graph/types/cell.hpp"
#include "graph/response.hpp"
//#include "app/bridge/iomodel.hpp"

namespace App {
namespace Bridge {

class ItemsModel : public QAbstractListModel
{
public:
    ItemsModel(QObject* parent=0) : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex& parent=QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const override;

    /*
     *  Returns the next valid item name for the given prefix
     */
    QString nextItemName(QString prefix="i") const;

    /*
     *  Update the model from the given response
     */
    void updateFrom(const Graph::Response& r);

protected:
    /*
     *  Generic item (can be used for either cell or instance)
     */
    struct Item
    {
        static Item Cell(const std::string& name, const std::string& expr)
        {
            return Item {
                CELL, QString::fromStdString(name), false, QString::fromStdString(expr),
                "", ""};
        }

        static Item Instance(const std::string& name, const std::string& sheet)
        {
            return Item {
                INSTANCE, QString::fromStdString(name), false, "", "",
                QString::fromStdString(sheet) };
        }

        enum { CELL, INSTANCE } type;
        QString name;

        bool cell_valid;
        QString cell_expr;
        QString cell_value;

        QString instance_sheet;

        //IOModel instance_io; TODO
    };

    enum ItemRole {
        TypeRole = Qt::UserRole + 1,
        NameRole,
        ValidRole,
        ExprRole,
        ValueRole,
    };

    QHash<int, QByteArray> roleNames() const override;

    QList<Item> items;

    // Meta-data for fast lookups
    std::map<Graph::ItemIndex, int> order;
    boost::bimap<Graph::ItemIndex, std::string> names;
};

}   // namespace Bridge
}   // namespace App
