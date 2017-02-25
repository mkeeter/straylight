#pragma once

#include "app/bridge/items.hpp"
#include "app/bridge/library.hpp"

#include "graph/response.hpp"

namespace App {
namespace Bridge {

class GraphModel;   // forward declaration

class SheetModel : public QObject
{
    Q_OBJECT
public:
    SheetModel(Graph::SheetIndex sheet, GraphModel* parent);

    /*
     *  Called to execute a reply from the graph thread
     */
    void updateFrom(const Graph::Response& r);

    /*
     *  Sets the sheet's target environment, which modifies
     *  which values are returned and the instance name property
     */
    void setEnv(const Graph::Env& env);

    Q_INVOKABLE QObject* itemsModel();
    Q_INVOKABLE QObject* libraryModel();

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

signals:
    void instanceNameChanged();
    void sheetNameChanged();

protected:
    GraphModel* graph;

    // Identity of this SheetModel
    const Graph::SheetIndex sheet;

    //  Env that we're looking at right now
    Graph::Env current_env;

    // Names of instances of this sheet
    std::map<Graph::InstanceIndex, QString> instance_names;

    // Properties
    QString _sheet_name;

public:
    void setInstanceName(const Graph::InstanceIndex& env, const std::string& n);
    QString getInstanceName() const;
    void setSheetName(const QString& s) {
        _sheet_name = s;
        emit(sheetNameChanged());
    }
    Q_PROPERTY(QString instanceName READ getInstanceName NOTIFY instanceNameChanged)
    Q_PROPERTY(QString sheetName MEMBER _sheet_name NOTIFY sheetNameChanged)

protected:
    ItemsModel items;
    LibraryModel library;
};

}   // namespace Bridge
}   // namespace App
