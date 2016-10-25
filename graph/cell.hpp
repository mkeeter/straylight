#pragma once

#include <map>
#include "types.hpp"

namespace Graph
{
struct Cell
{
    Cell(const Expr& expr) : expr(expr) {}

    Expr expr;
    std::map<Env, Value> values;
    std::map<Env, std::string> strs;
};
}   // namespace Graph
