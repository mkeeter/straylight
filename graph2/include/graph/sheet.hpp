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
     *  On destruction, free all items
     */
    ~Sheet();

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
    const Item& at(ItemIndex i) const
        { return items.at(i); }

    /*
     *  Get an item by index
     */
    const Item& at(const std::string& name) const
        { return items.at(indexOf(name)); }

    /*
     *  Get an index by name
     *  Fails if no such name exists
     */
    ItemIndex indexOf(const std::string& name) const
        { return names.left.at(name); }

    /*
     *  Checks whether an item exists, by name
     */
    bool hasItem(const std::string& name) const
        { return names.left.find(name) != names.left.end(); }

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
     *  Returns the next item index
     */
    ItemIndex nextIndex() const;

    /*  Master storage of all Items in the sheet  */
    std::map<ItemIndex, Item> items;

    /*  Map of names to item indices  */
    boost::bimap<std::string, ItemIndex> names;

    /*  Order of items for rendering  */
    std::list<ItemIndex> order;
};
