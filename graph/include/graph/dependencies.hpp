#pragma once

#include <map>
#include <set>

#include "graph/types/keys.hpp"

namespace Graph {

class Root;

class Dependencies
{
public:
    Dependencies(const Root& parent) : root(parent) {}

    /*
     *  Resets all stored dependencies
     */
    void reset();

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
     *  Lookup in the inverse map
     */
    const std::set<CellKey>& inverseDeps(const NameKey& k) const;

    /*
     *  Lookup in the forward map
     */
    const std::set<NameKey>& forwardDeps(const CellKey& k) const;

    /*
     *  Checks whether b is upstream of a
     */
    bool isUpstream(const CellKey& a, const CellKey& b) const
        {   return upstream.count(a) && upstream.at(a).count(b); }

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

}   // namespace Graph
