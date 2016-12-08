#pragma once

#include <string>
#include <map>

#include "graph/env.hpp"
#include "graph/value.hpp"

struct Cell
{
    Cell(const std::string& e) : expr(e) {}

    /*  Expression to be evaluated  */
    std::string expr;

    /*  type should be changed at the same time as expr  */
    enum Type { UNKNOWN, BASIC, INPUT, OUTPUT } type=UNKNOWN;

    /*  Per-environment value list  */
    std::map<Env, Value> values;
};
