#pragma once

#include <string>

struct s7_cell;

struct Value
{
    s7_cell* value=nullptr;
    std::string str="";
    bool valid=false;
};
