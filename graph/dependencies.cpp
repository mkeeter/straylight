#include "instance.hpp"
#include "sheet.hpp"
#include "dependencies.hpp"

namespace Graph
{

void Dependencies::insert(const CellKey& looker, const NameKey& lookee)
{
    forward[looker].insert(lookee);
    inverse[lookee].insert(looker);

    const auto ck = toCellKey(lookee);
    upstream[looker].insert(upstream[ck].begin(), upstream[ck].end());
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
