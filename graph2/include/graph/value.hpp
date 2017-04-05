#pragma once

#include <string>

#include "graph/interpreter.hpp"

namespace Graph {

struct Value
{
    Value(Interpreter::Value value, const std::string& str, bool valid)
        : value(value), str(str), valid(valid)
    {   /*  Nothing to do here  */ }

    /*  '(value $VALUE) or '(error $ERROR)  */
    Interpreter::Value value=nullptr;

    /*  String representation of the value */
    std::string str="";

    /*  Is this a value or an error?  */
    bool valid=false;
};

}   // namespace Graph
