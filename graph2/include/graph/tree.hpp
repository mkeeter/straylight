#pragma once

#include "graph/item.hpp"
#include "graph/index.hpp"
#include "graph/keynamestore.hpp"

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

protected:
    /*  Order of items for rendering  */
    std::list<ItemIndex> order;
};