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

    bool isInput() const { return false; }
    bool isOutput() const { return false; }

    std::string defaultExpr() const { return ""; }
};
}   // namespace Graph
