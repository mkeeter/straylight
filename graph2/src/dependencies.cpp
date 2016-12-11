#include "graph/dependencies.hpp"
#include "graph/root.hpp"

int Dependencies::insert(const CellKey& looker, const NameKey& lookee)
{
    forward[looker].insert(lookee);
    inverse[lookee].insert(looker);

    // If the target exists, then check for recursive lookups
    const auto sheet = root.getItem(lookee.first.back()).instance()->sheet;
    if (root.getSheet(sheet).hasItem(lookee.second))
    {
        const auto ck = root.toCellKey(lookee);
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

void Dependencies::clearAll(const SheetIndex& sheet, const ItemIndex& item)
{
    auto envs = root.envsOf(sheet);
    for (auto env : envs)
    {
        clear({env, item});
    }
}
