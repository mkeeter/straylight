#pragma once

#include <string>

#include "graph/interpreter.hpp"

struct Value
{
    Interpreter::ValueType value=nullptr;
    std::string str="";
    bool valid=false;
};
