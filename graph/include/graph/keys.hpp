#pragma once

#include <string>
#include <list>

#include "graph/index.hpp"
#include "graph/env.hpp"

namespace Graph {

/*
 *  Refers to a particular cell in the graph
 *
 *  Though a cell only exists once the library, here, we care about unique
 *  instances of the cell, so we use the environment (which is a list of
 *  ItemIndex) to localize the cell.
 */
typedef std::pair<Env, std::string> NameKey;
typedef std::pair<Env, CellIndex> CellKey;

}   // namespace Graph
