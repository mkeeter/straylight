#pragma once

#include <map>
#include <set>

#include "keys.hpp"

class Root;

class Dependencies
{
public:
    Dependencies(const Root& parent) : root(parent) {}

    /*
     *  Insert a lookup
     *  Returns 1 if the lookup is recursive, 0 otherwise
     */
    int insert(const CellKey& looker, const NameKey& lookee);

    /*
     *  Clear all recorded lookups
     *  This should be called before evaluating the cell
     */
    void clear(const CellKey& looker);

    /*
     *  Clear all references to the given sheet and item
     */
    void clearAll(const SheetIndex& sheet, const ItemIndex& item);

protected:
    /*
     *  Reference to parent root object, which we'll query for cell data
     */
    const Root& root;

    /*  forward[cell] records all of the lookups that cell made
     *  (by name, as that's how lookups work)    */
    std::map<CellKey, std::set<NameKey>> forward;

    /*  inverse[cell] records all of the things that looked up cell
     *  These are all the things that should be re-evaluated if cell changes.
     *
     *  Things are stored by name to make this lookup work when you insert a
     *  new Cell that matches a (previously invalid) name */
    std::map<NameKey, std::set<CellKey>> inverse;

    /*  upstream[cell] records everything that's upstream of cell
     *  This is the recursive union of forward[cell], and is used to
     *  detect circular dependencies. */
    std::map<CellKey, std::set<CellKey>> upstream;
};