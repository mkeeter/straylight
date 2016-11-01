#pragma once

#include <map>
#include "types.hpp"

namespace Graph
{
struct Sheet;
struct Cell;

struct Instance
{
    /*
     *  Default constructor
     *  (requires inputs to be populated elsewhere)
     */
    Instance(Sheet* sheet, Sheet* parent) : sheet(sheet), parent(parent) {}

    Sheet* const sheet;
    std::map<Cell*, Expr> inputs;

    Sheet* const parent;
};
}   // namespace Graph
