#pragma once

#include <map>

#include "graph/types/index.hpp"

namespace Graph {

struct Instance
{
    Instance(SheetIndex s) : sheet(s) {}

    /*  Map from input Cell indices to their expressions */
    std::map<CellIndex, std::string> inputs;

    /*  Represents an index into the Sheet library */
    SheetIndex sheet;
};

}   // namespace Graph
