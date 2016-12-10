#pragma once

#include <map>
#include <boost/bimap.hpp>

template <class Stored, class StoredIndex, class ParentIndex>
class KeyNameStore
{
public:
    /*
     *  Check to see whether we can insert the given item's name
     */
    bool canInsert(const std::string& name, ParentIndex parent={0}) const
    {
        // Require that the sheet exists and that the name isn't taken
        return name.size() &&
               storage.find(parent) != storage.end() &&
               names.left.find(std::make_pair(parent, name)) == names.left.end();
    }

    /*
     *  Store an item in the system
     */
    StoredIndex insert(const std::string& name, Stored s,
                       ParentIndex parent={0})
    {
        assert(canInsert(name, parent));

        auto name_key = std::make_pair(parent, name);
        StoredIndex next = {storage.rbegin()->first.i + 1};

        storage.insert({next, s});
        names.left.insert({name_key, next});

        return next;
    }

    /*
     *  Rename a stored item
     */
    void rename(StoredIndex s, const std::string& new_name)
    {
        ParentIndex parent = names.right.at(s).first;
        assert(canInsert(new_name, parent));

        names.right.erase(s);
        names.insert({{parent, new_name}, s});
    }

    /*
     *  Erase an item from the store
     */
    void erase(StoredIndex s)
    {
        auto nk = names.right.find(s);
        auto k = storage.find(s);

        assert(nk != names.right.end());
        assert(k != storage.end());

        storage.erase(k);
        names.right.erase(nk);
    }

    /*
     *  Get an item by index
     */
    const Stored& at(StoredIndex i) const
        { return storage.at(i); }

    /*
     *  Get a item's name
     */
    const std::string& nameOf(StoredIndex i) const
        { return names.right.at(i).second; }

    /*
     *  Returns all of the stored items that belong to the given parent
     */
    std::list<StoredIndex> childrenOf(ParentIndex parent) const
    {
        auto itr = names.left.upper_bound(std::make_pair(parent, ""));

        std::list<StoredIndex> out;
        while (itr != names.left.end() && itr->first.first == parent)
        {
            out.push_back((itr++)->second);
        }
        return out;
    }

protected:
    /*  Master storage for all items.  Indices are unique.  */
    std::map<StoredIndex, Stored> storage;

    /*  This maps name + parent pairs to stored items
     *
     *  Sorting of the pairs means that items with the same parent are
     *  stored together in order */
    boost::bimap<std::pair<ParentIndex, std::string>, StoredIndex> names;
};
