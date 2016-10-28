#pragma once

#include <string>
#include "s7.h"

namespace Graph
{
struct Value
{
    s7_pointer value=nullptr;
    std::string str="";
    bool valid=false;
};
}   // namespace Graph
