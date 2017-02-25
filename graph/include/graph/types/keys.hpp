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
    NameKey() : type(NAME_KEY_NONE) {}

    // Normal NameKeys can be constructed implicitly from {env, name}
    NameKey(const Env& e, const std::string& s)
        : std::pair<Env, std::string>(e, s), type(NAME_KEY_BASIC) {}

    // Dummy NameKeys have explicit constructors
    explicit NameKey(SheetIndex i, const std::string& s)
        : std::pair<Env, std::string>({InstanceIndex(i)}, s),
          type(NAME_KEY_SHEET) {}
    explicit NameKey(CellIndex i)
        : std::pair<Env, std::string>({InstanceIndex(i)}, ""),
          type(NAME_KEY_CELL) {}

    const enum { NAME_KEY_BASIC, NAME_KEY_SHEET,
                 NAME_KEY_CELL, NAME_KEY_NONE } type;
};
typedef std::pair<Env, CellIndex> CellKey;

}   // namespace Graph
