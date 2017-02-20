#pragma once

#include "app/bridge/items.hpp"
#include "app/bridge/sheets.hpp"

#include "graph/response.hpp"

namespace App {
namespace Bridge {

class SheetInstanceModel : public QObject
{
public:
    SheetInstanceModel(QObject* parent=nullptr) : QObject(parent) {}
    void updateFrom(const Graph::Response& r);

    /*
     *  Checks whether a name is valid
     *  Returns an empty string on success and an error message otherwise
     */
    Q_INVOKABLE QString checkItemName(QString name) const;
    Q_INVOKABLE QString checkItemRename(QString name) const;
    Q_INVOKABLE void renameItem(int item_index, QString name);
    Q_INVOKABLE QString nextItemName(int sheet_index) const;

    Q_INVOKABLE QString checkSheetName(QString name) const;
    Q_INVOKABLE QString checkSheetRename(QString name) const;
    Q_INVOKABLE void renameSheet(int sheet_index, QString name);
    Q_INVOKABLE void insertSheet(QString name);
    Q_INVOKABLE QString nextSheetName() const;
    Q_INVOKABLE void eraseSheet(int sheet_index);

    Q_INVOKABLE void insertCell(const QString& name);
    Q_INVOKABLE void setExpr(int cell_index, const QString& expr);
    Q_INVOKABLE void setInput(int instance_index, int cell_index,
                              const QString& expr);
    Q_INVOKABLE void eraseCell(int cell_index);

    Q_INVOKABLE void insertInstance(QString name,
                                    int target_sheet_index);
    Q_INVOKABLE void eraseInstance(int instance_index);

protected:
    ItemsModel items;
    SheetsModel library;
};

}   // namespace Bridge
}   // namespace App
