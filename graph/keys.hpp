#pragma once

#include "types.hpp"

namespace Graph
{
struct Cell;

typedef std::pair<Env, Name> NameKey;
typedef std::pair<Env, Cell*> CellKey;

NameKey toNameKey(const CellKey& c);
CellKey toCellKey(const NameKey& n);
}
