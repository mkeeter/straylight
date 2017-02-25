#pragma once

#include <QAbstractListModel>
#include <QString>
#include <QSet>

#include <boost/bimap.hpp>

#include "graph/response.hpp"

namespace App {
namespace Bridge {

class IOModel : public QAbstractListModel
{
    Q_OBJECT
public:
    IOModel(QObject* parent=0) : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex& parent=QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const override;

    /*
     *  Update the model from the given response
     */
    void updateFrom(const Graph::Response& r);

    /*
     *  Updates the target env, which determines which values are returned
     */
    void setEnv(const Graph::Env& e) { env = e; }

signals:
    void countChanged();

protected:
    /*
     *  Generic item (can be used for either cell or instance)
     */
    struct IOCell
    {
        enum { INPUT, OUTPUT } type;
        unsigned unique_index;
        QString name;

        std::map<Graph::Env, bool> valid;
        std::map<Graph::Env, QString> value;
        QString expr;
    };

    enum ItemRole {
        TypeRole = Qt::UserRole + 1,
        NameRole,
        ValidRole,
        ExprRole,
        ValueRole,
        UniqueIndexRole,
    };

    QHash<int, QByteArray> roleNames() const override;

    QList<IOCell> cells;

    int getCount() const { return cells.size(); }
    Q_PROPERTY(int count READ getCount NOTIFY countChanged)

    // Target env
    Graph::Env env;

    // Meta-data for fast lookups
    std::map<Graph::CellIndex, int> order;
};

}   // namespace Bridge
}   // namespace App
