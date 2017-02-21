#pragma once

#include "app/bridge/items.hpp"
#include "app/bridge/sheets.hpp"

#include "graph/response.hpp"

namespace App {
namespace Bridge {

class GraphModel;   // forward declaration

class SheetInstanceModel : public QObject
{
    Q_OBJECT
public:
    SheetInstanceModel(const Graph::Env& env, Graph::SheetIndex sheet,
                       GraphModel* parent);

    /*
     *  Called to execute a reply from the graph thread
     */
    void updateFrom(const Graph::Response& r);

    Q_INVOKABLE QObject* itemsModel() { return &items; }

    /*
     *  Inserts a cell with the next available valid name
     */
    Q_INVOKABLE void insertCell();

    /*
     *  Checks whether we can rename the given item, returning
     *  an error string if we cannot.
     */
    Q_INVOKABLE QString checkItemRename(unsigned i, const QString& str);

    /*
     *  Renames the target item
     */
    Q_INVOKABLE void renameItem(unsigned i, const QString& str);

#if 0
    /*
     *  Checks whether a name is valid
     *  Returns an empty string on success and an error message otherwise
     */
    Q_INVOKABLE QString checkItemName(QString name) const;
    Q_INVOKABLE QString checkItemRename(int item_index, QString name) const;
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
#endif

protected:
    GraphModel* graph;

    // Identity of this SheetInstanceModel
    const Graph::Env env;
    const Graph::SheetIndex sheet;

    ItemsModel items;
    SheetsModel library;
};

}   // namespace Bridge
}   // namespace App
