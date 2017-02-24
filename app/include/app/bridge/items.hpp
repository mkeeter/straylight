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
    Q_OBJECT
public:
    ItemsModel(QObject* parent=0) : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex& parent=QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const override;

    /*
     *  Returns the next valid item name for the given prefix
     */
    QString nextItemName(QString prefix="i") const;

    /*
     *  Checks to see if the given rename is valid
     *  Returns "" on success; otherwise returns an error string
     */
    QString checkItemRename(int i, const QString& str) const;

    /*
     *  Update the model from the given response
     */
    void updateFrom(const Graph::Response& r);

    /*
     *  Updates the target env, which determines which values are returned
     */
    void setEnv(const Graph::Env& env);

signals:
    void countChanged();

protected:
    /*
     *  Generic item (can be used for either cell or instance)
     */
    struct Item
    {
        static Item Cell(Graph::CellIndex i, const std::string& name,
                         const std::string& expr)
        {
            return Item {
                CELL, QString::fromStdString(name), i.i,
                std::map<Graph::Env, bool>(),
                QString::fromStdString(expr), std::map<Graph::Env, QString>(),
                "", ""};
        }

        static Item Instance(Graph::InstanceIndex i, const std::string& name,
                             const std::string& sheet)
        {
            return Item {
                INSTANCE, QString::fromStdString(name), i.i,
                std::map<Graph::Env, bool>(), "",
                std::map<Graph::Env, QString>(), "",
                QString::fromStdString(sheet) };
        }

        enum { CELL, INSTANCE } type;
        QString name;
        unsigned unique_index;

        std::map<Graph::Env, bool> cell_valid;
        QString cell_expr;
        std::map<Graph::Env, QString> cell_value;
        QString cell_type;

        QString instance_sheet;

        //IOModel instance_io; TODO
    };

    enum ItemRole {
        TypeRole = Qt::UserRole + 1,
        NameRole,
        ValidRole,
        ExprRole,
        ValueRole,
        IOTypeRole,
        UniqueIndexRole,
        SheetNameRole,
    };

    QHash<int, QByteArray> roleNames() const override;

    QList<Item> items;

    int getCount() const { return items.size(); }
    Q_PROPERTY(int count READ getCount NOTIFY countChanged)

    // Target env
    Graph::Env env;

    // Meta-data for fast lookups
    std::map<Graph::ItemIndex, int> order;
    boost::bimap<Graph::ItemIndex, std::string> names;
};

}   // namespace Bridge
}   // namespace App
