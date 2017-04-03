#pragma once

#include <string>

#include "graph/interpreter.hpp"

namespace Graph {

struct Value
{
    Value(Interpreter::Value value, const std::string& str, bool valid)
        : value(value), str(str), valid(valid)
    {   /*  Nothing to do here  */ }

    Interpreter::Value value=nullptr;
    std::string str="";
    bool valid=false;
};

}   // namespace Graph
