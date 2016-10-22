#include "instance.hpp"
#include "sheet.hpp"
#include "dependencies.hpp"

namespace Graph
{

int Dependencies::insert(const CellKey& looker, const NameKey& lookee)
{
    forward[looker].insert(lookee);
    inverse[lookee].insert(looker);

    const auto ck = toCellKey(lookee);
    upstream[looker].insert(upstream[ck].begin(), upstream[ck].end());

    return upstream[ck].count(looker) > 0;
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

}   // namespace Graph
