#include "graph/instance.hpp"
#include "graph/sheet.hpp"
#include "graph/dependencies.hpp"

namespace Graph
{

int Dependencies::insert(const CellKey& looker, const NameKey& lookee)
{
    forward[looker].insert(lookee);
    inverse[lookee].insert(looker);

    // If this value exists, then check for recursive lookups
    if (lookee.first.back()->sheet->cells.left.count(lookee.second))
    {
        const auto ck = toCellKey(lookee);
        upstream[looker].insert(upstream[ck].begin(), upstream[ck].end());

        return upstream[ck].count(looker) > 0;
    }

    // Otherwise return 0
    return 0;
}

void Dependencies::clear(const CellKey& looker)
{
    for (auto k : forward[looker])
    {
        inverse[k].erase(looker);
    }
    forward[looker].clear();
    upstream[looker] = {looker};
}

void Dependencies::clearAll(const Cell* cell)
{
    std::list<CellKey> keys;

    for (auto k : forward)
    {
        if (k.first.second == cell)
        {
            keys.push_back(k.first);
        }
    }

    for (auto k : keys)
    {
        clear(k);
    }
}

}   // namespace Graph
