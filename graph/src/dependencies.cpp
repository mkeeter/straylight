#include "graph/dependencies.hpp"
#include "graph/root.hpp"

namespace Graph {

void Dependencies::reset()
{
    forward.clear();
    inverse.clear();
    upstream.clear();
}

int Dependencies::insert(const CellKey& looker, const NameKey& lookee)
{
    forward[looker].insert(lookee);
    inverse[lookee].insert(looker);

    // If the target exists, then check for recursive lookups
    if (lookee.type == NameKey::NAME_KEY_BASIC)
    {
        auto sheet = root.getTree().at(lookee.first.back()).instance()->sheet;
        if (root.getTree().hasItem(sheet, lookee.second) &&
            root.getTree().at(sheet, lookee.second).cell())
        {
            const auto ck = root.toCellKey(lookee);
            upstream[looker].insert(upstream[ck].begin(), upstream[ck].end());

            return upstream[ck].count(looker) > 0;
        }
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

const std::set<CellKey>& Dependencies::inverseDeps(const NameKey& k) const
{
    const static std::set<CellKey> empty;
    return inverse.count(k) ? inverse.at(k) : empty;
}

}   // namespace Graph
