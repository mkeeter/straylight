#pragma once

#include <map>
#include "types.hpp"

namespace Graph
{
struct Cell
{
    Cell(const Expr& expr);

    Expr expr;
    std::map<Env, Value> results;

    bool isInput() const;
    bool isOutput() const;
};
}   // namespace Graph
