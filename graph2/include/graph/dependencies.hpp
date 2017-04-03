#pragma once

#include <map>
#include <set>
#include <boost/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>

#include "graph/cell.hpp"
#include "graph/id.hpp"

namespace Graph
{

class Dependencies
{
public:
    typedef std::pair<CellId, Cell::Env> CellKey;
    typedef std::pair<SheetId, std::string> NameKey;

    /*
     *  Resets all stored dependencies
     */
    void clear();

    /*
     *  Record a failed lookup by name
     */
    void insert(const CellKey& looker, const NameKey& lookee);

    /*
     *  Record a successful lookup
     *  Returns 1 if the lookup is recursive, 0 otherwise
     */
    bool insert(const CellKey& looker, const CellKey& lookee);

    /*
     *  Clear all recorded lookups by the given looker
     *  This should be called before evaluating the cell
     */
    void erase(const CellKey& looker);

protected:
    /*  lookups.left[$FOO] are successful lookups by $FOO
     *  lookups.right[$BAR] are things that looked up $BAR  */
    boost::bimap<boost::bimaps::multiset_of<CellKey>,
                 boost::bimaps::multiset_of<CellKey>> lookups;

    /*  lookups.left[$FOO] are failed lookups by $FOO
     *  lookups.right[$BAR] are things that looked up $BAR  */
    boost::bimap<boost::bimaps::multiset_of<CellKey>,
                 boost::bimaps::multiset_of<NameKey>> failed;

    /*  upstream[cell] records everything that's upstream of cell
     *  This is the recursive union of lookups.left[cell], and is used to
     *  detect circular dependencies. */
    std::map<CellKey, std::set<CellKey>> upstream;

public:
    /*
     *  Returns a pair of iterators spanning the range of forward lookups,
     *  i.e. cells looked up by the argument
     */
    typedef decltype(lookups)::left_const_iterator forward_iterator;
    std::pair<forward_iterator, forward_iterator>
        forward(const CellKey& looker) const;

    /*
     *  Returns a pair of iterators spanning the range of inverse lookups,
     *  i.e. cells that looked up the argument
     */
    typedef decltype(lookups)::right_const_iterator inverse_iterator;
    std::pair<inverse_iterator, inverse_iterator>
        inverse(const CellKey& lookee) const;

    typedef decltype(failed)::right_const_iterator failed_iterator;
    std::pair<failed_iterator, failed_iterator>
        inverse(const NameKey& lookee) const;

};

}   // namespace Graph
