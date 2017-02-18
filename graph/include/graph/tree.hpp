#pragma once

#include "graph/types/index.hpp"
#include "graph/types/env.hpp"
#include "graph/types/keys.hpp"

#include "graph/item.hpp"
#include "graph/keynamestore.hpp"

#include "picojson/picojson.h"

namespace Graph {

class TreeSerializer;

class Tree : public KeyNameStore<Item, ItemIndex, SheetIndex>
{
public:
    /*
     *  On creation, insert item 0, which is an instance pointing to sheet 0
     *  with no parent and no name
     */
    Tree();

    /*
     *  On destruction, free all items
     */
    ~Tree();

    /*
     *  Clears storage, then inserts a sheet with id ROOT_SHEET and instance
     *  of that sheet with id ROOT_INSTANCE.
     */
    void reset();

    /*
     *  Insert a new cell with the given expression
     */
    CellIndex insertCell(const SheetIndex& parent, const std::string& name,
                         const std::string& expr);
    void insertCell(const SheetIndex& parent, const CellIndex& cell,
                    const std::string& name, const std::string& expr);

    /*
     *  Insert a new instance of the given sheet
     */
    InstanceIndex insertInstance(const SheetIndex& parent,
                                 const std::string& name,
                                 const SheetIndex& target);
    void insertInstance(const SheetIndex& parent,
                        const InstanceIndex& instance,
                        const std::string& name,
                        const SheetIndex& target);

    /*
     *  Checks to see whether we can insert the given instance
     *  (looking for recursive loops)
     */
    bool canInsertInstance(const SheetIndex& parent,
                           const SheetIndex& target) const;

    /*
     *  Iterate over items for a given parent
     */
    const std::list<ItemIndex>& iterItems(const SheetIndex& parent) const;

    /*
     *  Returns all the environments that the given sheet is instanced into
     */
    std::list<Env> envsOf(const SheetIndex& s) const;

    /*
     *  Returns all of the instances that use the given sheet
     */
    std::list<InstanceIndex> instancesOf(const SheetIndex& s) const;

    /*
     *  Erase an item from the tree and the order
     */
    void erase(const ItemIndex& i);

    /*
     *  Returns a list of cell keys relative to the given sheet
     */
    std::list<CellKey> cellsOf(const SheetIndex& s) const;

    /*
     *  Exports the graph to any object implementing the TreeSerializer API
     *  This is commonly used to render into a UI in a immediate style
     */
    void serialize(TreeSerializer* s) const;

    /*
     *  Encode the entire graph in JSON
     *
     *  This is used to save files to disk with an appropriate encoder
     */
    std::string toString() const;

    /*
     *  Deserializes from string (which should be JSON)
     *
     *  The existing graph must contain only the root sheet and instance.
     *  Returns an error string on failure or the empty string on success
     */
    std::string fromString(const std::string& str);

    /*
     *  Returns all of the sheets above (contained within) the given env
     *
     *  Note that this doesn't tell us whether we can insert an instance
     *  of these sheets, as this could create a recursive loop; use
     *  tree.canInsertInstance to check.
     */
    std::list<SheetIndex> sheetsAbove(const Env& e) const;

    /*
     *  Checks to see whether the given env is valid
     *  (i.e. that every item in it exists and is an instance)
     */
    bool checkEnv(const Env& env) const;

    const static SheetIndex ROOT_SHEET;
    const static InstanceIndex ROOT_INSTANCE;

protected:
    /*
     *  Exports the graph to any object implementing the TreeSerializer API
     */
    void serialize(TreeSerializer* s, const Env& env) const;

    /*
     *  Export the given sheet to JSON (recursively)
     */
    picojson::value toJson(SheetIndex sheet) const;

    /*
     *  Deserializes a particular sheet (recursively)
     *
     *  Returns an error string on error, empty string otherwise
     */
    std::string fromJson(SheetIndex sheet, const picojson::value& value);

    ////////////////////////////////////////////////////////////////////////////

    /*
     *  Rendering order for items in a given sheet
     */
    std::map<SheetIndex, std::list<ItemIndex>> order;
};

}   // namespace Graph
