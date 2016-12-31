#pragma once

#include "graph/item.hpp"
#include "graph/index.hpp"
#include "graph/keynamestore.hpp"
#include "graph/env.hpp"
#include "graph/keys.hpp"

namespace Graph {

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
     *  Insert a new cell with the given expression
     */
    CellIndex insertCell(const SheetIndex& parent, const std::string& name,
                         const std::string& expr);

    /*
     *  Insert a new instance of the given sheet
     */
    InstanceIndex insertInstance(const SheetIndex& parent,
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

protected:
    /*
     *  Rendering order for items in a given sheet
     */
    std::map<SheetIndex, std::list<ItemIndex>> order;
};

}   // namespace Graph
