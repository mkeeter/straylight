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
    /*  Library of child sheets  */
    std::map<Name, std::unique_ptr<Sheet>> library;

    /*  Instances of sheets (from library or parent's library) */
    std::map<Name, std::unique_ptr<Instance>> instances;

    /*  Cells local to this sheet   */
    boost::bimap<Name, std::unique_ptr<Cell>> cells;

    /*
     *  Returns a list of cells that should be inputs,
     *  along with their default expression
     */
    std::list<std::pair<Cell*, Expr>> inputs() const;
};
}   // namespace Graph
