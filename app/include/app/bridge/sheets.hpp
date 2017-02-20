#pragma once

#include <QAbstractListModel>

#include "graph/response.hpp"

namespace App {
namespace Bridge {

struct Sheet
{
    QString name;
    bool editable;
    bool insertable;
};

class SheetsModel : public QAbstractListModel
{
public:
    SheetsModel(QObject* parent=0) : QAbstractListModel(parent) {}

    /*
     *  Update the model state from the given response
     */
    void updateFrom(const Graph::Response& r);

    QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const override;
    int rowCount(const QModelIndex& parent=QModelIndex()) const override;

protected:
    enum SheetRole {
        NameRole = Qt::UserRole + 1,
        EditableRole,
        InsertableRole,
    };

    QHash<int, QByteArray> roleNames() const override;

    QList<Sheet> sheets;
};

}   // namespace Bridge
}   // namespace App

