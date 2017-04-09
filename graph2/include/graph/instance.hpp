#pragma once

#include <map>

#include "graph/node.hpp"

namespace Graph {

struct Instance : public Node
{
    Instance(const SheetId target, const SheetId parent);

    const SheetId target;
    /*  Cells that are of type INPUT or OUTPUT in the target sheet
     *  appear in the instance.  INPUT cells are inputs with expressions
     *  in the map below ('inputs'); OUTPUT cells just show their value.
     *
     *  This map allows us to lift them a level farther up the graph.
     *  INPUT cells can be lifted as INPUTs or OUTPUTs; OUTPUT cells can
     *  only be lifted as OUTPUTs.  */
    std::map<CellId, Cell::Type> lift;

    /*  Expressions for inputs  */
    std::map<CellId, std::string> inputs;
};

}   // namespace Graph
