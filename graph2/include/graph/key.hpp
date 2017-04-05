#pragma once

#include <utility>

#include "graph/id.hpp"
#include "graph/cell.hpp"

namespace Graph
{

struct CellKey
{
    CellKey(CellId i, Cell::Env e);

    bool operator==(const CellKey& other) const;
    bool operator!=(const CellKey& other) const;
    bool operator<(const CellKey& other) const;

    /*
     *  Returns true if *this is a specialization of other, i.e.
     *  they have the same id and other.env is a prefix of this->env.
     */
    bool specializes(const CellKey& other) const;

    CellId id;
    Cell::Env env;
};

typedef std::pair<SheetId, std::string> NameKey;

}   // namespace Graph
