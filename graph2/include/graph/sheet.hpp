#pragma once

#include <map>
#include <list>
#include <boost/bimap.hpp>

#include "graph/item.hpp"
#include "graph/index.hpp"

class Sheet
{
public:
    /*
     *  Check to see whether we can insert a cell or instance
     */
    bool canInsert(std::string name) const;

    /*
     *  Insert a new cell with the given expression
     */
    ItemIndex insertCell(std::string name, std::string expr);

    /*
     *  Insert a new instance of the given sheet
     */
    ItemIndex insertInstance(std::string name, SheetIndex sheet);

    /*
     *  Rename a cell or instance
     */
    void rename(ItemIndex item, std::string new_name);

    /*
     *  Erase a cell or instance
     */
    void erase(ItemIndex item);

    /*
     *  Get an item by index
     */
    const Item& operator[](ItemIndex i) const
        { return items.at(i).i; }

    /*
     *  Get a item's name
     */
    const std::string& nameOf(ItemIndex i) const
        { return names.right.at(i); }

    /*
     *  Iterate over items in order
     */
    const std::list<ItemIndex>& iterItems() const
        { return order; }

protected:
    /*
     *  RAII handle for an Item
     *  (which usually needs manual deallocation)
     */
    struct ItemHandle
    {
        ItemHandle(Item i) : i(i) {}
        ~ItemHandle() { i.dealloc(); }
        Item i;
    };

    /*  Master storage of all Items in the sheet  */
    std::map<ItemIndex, ItemHandle> items;

    /*  Map of names to item indices  */
    boost::bimap<std::string, ItemIndex> names;

    /*  Order of items for rendering  */
    std::list<ItemIndex> order;
};
