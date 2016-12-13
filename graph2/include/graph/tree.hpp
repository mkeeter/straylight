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
    ItemIndex insertCell(std::string name, std::string expr,
                         SheetIndex parent);

    /*
     *  Insert a new instance of the given sheet
     */
    ItemIndex insertInstance(std::string name, SheetIndex sheet,
                             SheetIndex parent);

    /*
     *  Iterate over items for a given parent
     */
    const std::list<ItemIndex>& iterItems(const SheetIndex& parent) const;

    /*
     *  Returns all the environments that the given sheet is instanced into
     */
    std::list<Env> envsOf(const SheetIndex& s) const;

protected:
    /*
     *  Rendering order for items in a given sheet
     */
    std::map<SheetIndex, std::list<ItemIndex>> order;
};
