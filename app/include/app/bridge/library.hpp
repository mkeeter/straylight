#pragma once

#include <QAbstractListModel>

#include <boost/bimap.hpp>
#include "graph/response.hpp"

namespace App {
namespace Bridge {

////////////////////////////////////////////////////////////////////////////////

struct Sheet
{
    QString name;
    bool editable;
    bool insertable;
    Graph::SheetIndex sheet_index;
};

////////////////////////////////////////////////////////////////////////////////

class LibraryModel : public QAbstractListModel
{
    Q_OBJECT
public:
    LibraryModel(QObject* parent=0) : QAbstractListModel(parent) {}

    /*
     *  Returns the next valid item name for the given prefix
     */
    QString nextSheetName(QString prefix="I") const;

    /*
     *  Checks to see if the given rename is valid
     *  Returns "" on success; otherwise returns an error string
     */
    QString checkSheetRename(int i, const QString& str) const;

    /*
     *  Update the model state from the given response
     */
    void updateFrom(const Graph::Response& r);

    QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const override;
    int rowCount(const QModelIndex& parent=QModelIndex()) const override;

signals:
    void countChanged();

protected:
    enum SheetRole {
        NameRole = Qt::UserRole + 1,
        EditableRole,
        InsertableRole,
        SheetIndexRole,
        IsLastRole,
    };

    QHash<int, QByteArray> roleNames() const override;

    QList<Sheet> sheets;
    int getCount() const { return sheets.size(); }
    Q_PROPERTY(int count READ getCount NOTIFY countChanged)

    // Meta-data for fast lookups
    std::map<Graph::SheetIndex, int> order;
    boost::bimap<Graph::SheetIndex, std::string> names;
};

}   // namespace Bridge
}   // namespace App

