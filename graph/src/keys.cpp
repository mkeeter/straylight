#include "graph/keys.hpp"
#include "graph/instance.hpp"
#include "graph/sheet.hpp"
#include "graph/cell.hpp"

namespace Graph
{

NameKey toNameKey(const CellKey& c)
{
    return {c.first, c.first.back()->sheet->cells.right.at(c.second)};
}

CellKey toCellKey(const NameKey& c)
{
    return {c.first, c.first.back()->sheet->cells.left.at(c.second)};
}

bool isCellKey(const NameKey& c)
{
    return c.first.back()->sheet->cells.left.count(c.second);
}

}
