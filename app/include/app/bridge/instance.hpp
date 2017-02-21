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
    Q_INVOKABLE QObject* libraryModel() { return &library; }

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

    /*
     *  Sets the target cell's expression
     */
    Q_INVOKABLE void setExpr(unsigned cell_index, const QString& expr);

    /*
     *  Erases the target item
     */
    Q_INVOKABLE void eraseCell(unsigned cell_index);
    Q_INVOKABLE void eraseInstance(unsigned cell_index);

    /*
     *  Inserts an instance of a particular sheet
     */
    Q_INVOKABLE void insertInstance(unsigned target_sheet_index);

    /*
     *  Erase a sheet from this sheet instance's library
     */
    Q_INVOKABLE void eraseSheet(unsigned sheet_index);

    /*
     *  Renames the target sheet
     */
    Q_INVOKABLE void renameSheet(unsigned sheet_index, const QString& name);

    /*
     *  Inserts a new sheet with a default-constructed name
     */
    Q_INVOKABLE void insertSheet();

    /*
     *  Checks whether the given sheet can be renamed,
     *  returning an error string
     */
    Q_INVOKABLE QString checkSheetRename(unsigned i, const QString& name) const;

#if 0
    /*
     *  Checks whether a name is valid
     *  Returns an empty string on success and an error message otherwise
     */
    Q_INVOKABLE QString checkItemName(QString name) const;
    Q_INVOKABLE void renameItem(int item_index, QString name);
    Q_INVOKABLE QString nextItemName(int sheet_index) const;

    Q_INVOKABLE QString checkSheetName(QString name) const;
    Q_INVOKABLE QString checkSheetRename(QString name) const;
    Q_INVOKABLE void renameSheet(int sheet_index, QString name);
    Q_INVOKABLE void insertSheet(QString name);
    Q_INVOKABLE QString nextSheetName() const;

    Q_INVOKABLE void insertCell(const QString& name);
    Q_INVOKABLE void setExpr(int cell_index, const QString& expr);
    Q_INVOKABLE void setInput(int instance_index, int cell_index,
                              const QString& expr);
    Q_INVOKABLE void eraseCell(int cell_index);

    Q_INVOKABLE void eraseInstance(unsigned instance_index);
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
