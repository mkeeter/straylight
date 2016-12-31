#pragma once

#include <string>

#include "graph/ptr.hpp"

namespace Graph {

struct Value
{
    Value(ValuePtr value, std::string str, bool valid)
        : value(value), str(str), valid(valid) {}

    ValuePtr value=nullptr;
    std::string str="";
    bool valid=false;
};

}   // namespace Graph
