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

    /*  Expression to be evaluated! */
    Expr expr;
    /*  type should be changed at the same time as expr  */
    enum Type { UNKNOWN, BASIC, INPUT, OUTPUT } type=UNKNOWN;

    std::map<Env, Value> values;

    Sheet* const parent;
};
}   // namespace Graph
