#include "sheet.hpp"
#include "cell.hpp"
#include "instance.hpp"

namespace Graph
{

Sheet::~Sheet()
{
    for (auto c : cells.left)
    {
        delete c.second;
    }
}
}
