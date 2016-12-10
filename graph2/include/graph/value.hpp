#pragma once

#include <string>

#include "graph/ptr.hpp"

struct Value
{
    ValuePtr value=nullptr;
    std::string str="";
    bool valid=false;
};
