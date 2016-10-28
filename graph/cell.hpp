#pragma once

#include <map>

#include "types.hpp"
#include "value.hpp"

namespace Graph
{
struct Cell
{
    Cell(const Expr& expr) : expr(expr) {}

    Expr expr;
    std::map<Env, Value> values;
};
}   // namespace Graph
