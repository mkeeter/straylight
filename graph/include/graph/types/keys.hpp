#pragma once

#include <string>
#include <list>

#include "graph/types/index.hpp"
#include "graph/types/env.hpp"

namespace Graph {

/*
 *  Refers to a particular cell in the graph
 *
 *  Though a cell only exists once the library, here, we care about unique
 *  instances of the cell, so we use the environment (which is a list of
 *  ItemIndex) to localize the cell.
 */
struct NameKey : public std::pair<Env, std::string>
{
    NameKey() {}
    NameKey(const Env& e, const std::string& s)
        : std::pair<Env, std::string>(e, s) {}
    NameKey(SheetIndex i, const std::string& s)
        : std::pair<Env, std::string>({InstanceIndex(i)}, s) {}
    NameKey(CellIndex i)
        : std::pair<Env, std::string>({InstanceIndex(i)}, "") {}
    // TODO: better way to identify these different variations?
};
typedef std::pair<Env, CellIndex> CellKey;

}   // namespace Graph
