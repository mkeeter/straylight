#include "graph/dependencies.hpp"

namespace Graph
{

void Dependencies::clear()
{
    lookups.clear();
    failed.clear();
}

void Dependencies::insert(const CellKey& looker, const NameKey& lookee)
{
    failed.left.insert({looker, lookee});
}

bool Dependencies::insert(const CellKey& looker, const CellKey& lookee)
{
    const bool recursive = isRecursive(looker, lookee);
    lookups.left.insert({looker, lookee});
    return recursive;
}

bool Dependencies::isRecursive(const CellKey& looker, const CellKey& lookee) const
{
    std::list<const CellKey> todo = {lookee};
    std::set<const CellKey> seen;

    // Walk through everything that's upstream of lookee, searching for
    // a situation which creates a recursive loop with looker
    while (todo.size())
    {
        if (looker == todo.front())
        {
            return true;
        }
        auto f = todo.front();
        todo.pop_front();

        // If we haven't checked this node yet, then add all of its
        // upstream nodes to the list of things to check
        if (seen.find(f) == seen.end())
        {
            seen.insert(f);
            auto up = lookups.left.equal_range(f);

            for (auto itr = up.first; itr != up.second; ++itr)
            {
                todo.push_back(itr->second);
            }
        }
    }
    return false;
}

void Dependencies::erase(const CellKey& looker)
{
    lookups.left.erase(looker);
    failed.left.erase(looker);
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

Dependencies::inverse_iterator Dependencies::find(const CellKey& lookee) const
{
    return lookups.right.find(lookee);
}

Dependencies::inverse_iterator Dependencies::end() const
{
    return lookups.right.end();
}

}   // namespace Graph
