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
    ~Sheet();

    /*  Library of child sheets  */
    std::map<Name, std::unique_ptr<Sheet>> library;

    /*  Instances of sheets (from library or parent's library) */
    std::map<Name, std::unique_ptr<Instance>> instances;

    /*  Cells local to this sheet   */
    boost::bimap<Name, Cell*> cells;

    /*
     *  Returns a list of cells that should be inputs,
     *  along with their default expression
     */
    std::list<Cell*> inputs() const { return {}; };
    std::list<Cell*> outputs() const { return {}; };
};
}   // namespace Graph
