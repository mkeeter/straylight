#pragma once

#include <QAbstractItemModel>

#include "graph/response.hpp"

namespace App {
namespace Bridge {

struct Sheet
{
    QString name;
    bool editable;
    bool insertable;
};

class SheetsModel : public QAbstractItemModel
{
public:
    SheetsModel(QObject* parent=0);

    void addSheet(const Sheet& sheet);
    int rowCount(const QModelIndex& parent=QModelIndex()) const;

    /*
     *  Update the model state from the given response
     */
    void operator()(const Graph::Response& r);

    QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const;

protected:
    enum SheetRole {
        NameRole = Qt::UserRole + 1,
        EditableRole,
        InsertableRole,
    };

    QList<Sheet> sheets;
};

}   // namespace Bridge
}   // namespace App

