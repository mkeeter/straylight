#pragma once

#include <boost/bimap.hpp>

#include "index.hpp"
#include "keys.hpp"

#include "keynamestore.hpp"

namespace Graph {

struct Sheet {
    /*
     *  Dummy struct for Library
     *  Sheet members are stored in Tree
     */
};

class Library : public KeyNameStore<Sheet, SheetIndex, SheetIndex>
{
public:
    /*
     *  By default, construct sheet 0
     *  Sheet 0 has no parent and no name
     */
    Library();

    /*
     *  Inserts the given sheet, returning its index
     */
    SheetIndex insert(SheetIndex parent, const std::string& name);
};

}   // namespace Graph
