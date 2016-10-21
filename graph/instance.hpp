#pragma once

#include <map>
#include "types.hpp"

namespace Graph
{
struct Sheet;
struct Cell;

struct Instance
{
    Instance(Sheet* sheet);
    ~Instance();

    Sheet* sheet;
    std::map<Cell*, Expr> inputs;
};
}   // namespace Graph
