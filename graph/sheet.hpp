#pragma once

#include <map>
#include <memory>
#include <list>
#include <set>
#include <boost/bimap.hpp>

#include "types.hpp"

namespace Graph
{
struct Instance;
struct Cell;

struct Sheet
{
    Sheet(Sheet* parent) : parent(parent) {}
    ~Sheet();

    /*  Library of child sheets  */
    boost::bimap<Name, Sheet*> library;

    /*
     *  Check whether the given pointer is a stored instance
     *  Otherwise, return nullptr
     */
    Instance* isInstance(void* ptr) const;

    /*
     *  Check whether the given pointer is a stored cell
     *  Otherwise, return nullptr
     */
    Cell* isCell(void* ptr) const;

    /*
     *  Returns a list of cells that should be inputs,
     *  along with their default expression
     */
    std::list<Cell*> inputs() const { return {}; };
    std::list<Cell*> outputs() const { return {}; };

    /*  Instances of sheets (from library or parent's library) */
    boost::bimap<Name, Instance*> instances;

    /*  Cells local to this sheet   */
    boost::bimap<Name, Cell*> cells;

    /*  Cells and instances in order that they should be drawn  */
    std::list<void*> order;

    Sheet* const parent;
};
}   // namespace Graph
