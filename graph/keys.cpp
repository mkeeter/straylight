#pragma once

#include "keys.hpp"
#include "instance.hpp"
#include "sheet.hpp"
#include "cell.hpp"

namespace Graph
{

NameKey toNameKey(const CellKey& c)
{
    return {c.first, c.first.back()->sheet->cells.right[c.second]};
}

CellKey toCellKey(const NameKey& c)
{
    return {c.first, c.first.back()->sheet->cells.left[c.second].get()};
}

}
