#include "graph/dependencies.hpp"

namespace Graph
{

void Dependencies::clear()
{
    lookups.clear();
    failed.clear();
    upstream.clear();
}

void Dependencies::insert(const CellKey& looker, const NameKey& lookee)
{
    failed.left.insert({looker, lookee});
}

bool Dependencies::insert(const CellKey& looker, const CellKey& lookee)
{
    lookups.left.insert({looker, lookee});

    const auto& u = upstream[lookee];
    upstream[looker].insert(u.begin(), u.end());
    return upstream[lookee].count(looker) > 0;
}

void Dependencies::erase(const CellKey& looker)
{
    lookups.left.erase(looker);
    failed.left.erase(looker);
    upstream[looker] = {looker};
}

std::pair<Dependencies::forward_iterator, Dependencies::forward_iterator>
Dependencies::forward(const CellKey& looker) const
{
    return lookups.left.equal_range(looker);
}

std::pair<Dependencies::inverse_iterator, Dependencies::inverse_iterator>
Dependencies::inverse(const CellKey& looker) const
{
    return lookups.right.equal_range(looker);
}

std::pair<Dependencies::failed_iterator, Dependencies::failed_iterator>
Dependencies::inverse(const NameKey& lookee) const
{
    return failed.right.equal_range(lookee);
}

}   // namespace Graph
