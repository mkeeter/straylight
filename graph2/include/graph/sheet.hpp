#pragma once

#include <map>
#include <set>
#include <boost/bimap.hpp>

#include "graph/node.hpp"
#include "graph/id.hpp"

namespace Graph {

// Forward declarations
struct Cell;
struct Instance;

struct Sheet : public Node
{
    /*
     *  Constructor for the root-level sheet
     */
    Sheet(const SheetId parent);

    /*  Items that are hierarchically within this Sheet
     *  (they're actually stored as unique_ptrs in the Graph ) */
    boost::bimap<std::string, CellId> cells;
    boost::bimap<std::string, InstanceId> instances;
    boost::bimap<std::string, SheetId> sheets;

    /*  Where is this Sheet used as an instance?  */
    std::set<InstanceId> uses;
};

}   // namespace Graph
