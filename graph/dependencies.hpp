#pragma once

#include <map>
#include <set>

#include "types.hpp"
#include "keys.hpp"

namespace Graph
{
struct Cell;

struct Dependencies
{
    /*
     *  Insert a lookup
     */
    void insert(const CellKey& looker, const NameKey& lookee);

    /*
     *  Clear all recorded lookups
     *  This should be called before evaluating the cell
     */
    void clear(const CellKey& looker);

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

}   // namespace Graph
