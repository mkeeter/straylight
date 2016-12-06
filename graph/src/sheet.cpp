#include "graph/sheet.hpp"
#include "graph/cell.hpp"
#include "graph/instance.hpp"

namespace Graph
{
Sheet::~Sheet()
{
    for (auto c : cells.left)
    {
        delete c.second;
    }
    for (auto s : library.left)
    {
        delete s.second;
    }
    for (auto s : instances.left)
    {
        delete s.second;
    }
}

Cell* Sheet::isCell(void* ptr) const
{
    auto c = static_cast<Cell*>(ptr);
    return cells.right.count(c) ? c : nullptr;
}

Instance* Sheet::isInstance(void* ptr) const
{
    auto i = static_cast<Instance*>(ptr);
    return instances.right.count(i) ? i : nullptr;
}

}
