#pragma once

#include <map>

#include "types.hpp"
#include "value.hpp"

namespace Graph
{
struct Sheet;

struct Cell
{
    Cell(const Expr& expr, Sheet* parent) : expr(expr), parent(parent) {}

    Expr expr;
    std::map<Env, Value> values;

    Sheet* const parent;
};
}   // namespace Graph
