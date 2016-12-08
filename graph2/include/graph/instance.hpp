#pragma once

#include <map>

#include "index.hpp"

struct Instance
{
    Instance(SheetIndex s) : sheet(s) {}

    /*  Map from input Cell indices to their expressions */
    std::map<ItemIndex, std::string> inputs;

    /*  Represents an index into the Sheet library */
    SheetIndex sheet;
};
