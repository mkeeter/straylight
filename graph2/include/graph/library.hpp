#pragma once

#include <boost/bimap.hpp>

#include "index.hpp"
#include "keys.hpp"
#include "sheet.hpp"

#include "keynamestore.hpp"

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
    SheetIndex insert(const std::string& name, SheetIndex parent={0});
};
