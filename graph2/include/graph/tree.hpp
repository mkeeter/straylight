#pragma once

#include "graph/item.hpp"
#include "graph/index.hpp"
#include "graph/keynamestore.hpp"
#include "graph/env.hpp"

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
    ItemIndex insertCell(const SheetIndex& parent, const std::string& name,
                         const std::string& expr);

    /*
     *  Insert a new instance of the given sheet
     */
    ItemIndex insertInstance(const SheetIndex& parent, const std::string& name,
                             const SheetIndex& target);

    /*
     *  Checks to see whether we can insert the given cell
     */
    bool canInsertCell(const SheetIndex& parent,
                       const std::string& name) const
        { return canInsert(parent, name); }

    /*
     *  Checks to see whether we can insert the given instance
     *  (checking both name collisions and recursive loops)
     */
    bool canInsertInstance(const SheetIndex& parent, const SheetIndex& target,
                           const std::string& name) const;

    /*
     *  Check whether we can rename a cell or instance
     */
    bool canRename(const ItemIndex& item, const std::string& new_name) const;

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
    std::list<ItemIndex> instancesOf(const SheetIndex& s) const;

protected:
    /*
     *  Rendering order for items in a given sheet
     */
    std::map<SheetIndex, std::list<ItemIndex>> order;
};
