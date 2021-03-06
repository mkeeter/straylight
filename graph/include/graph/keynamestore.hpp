#pragma once

#include <map>
#include <boost/bimap.hpp>

namespace Graph {

template <class Stored, class StoredIndex, class ParentIndex>
class KeyNameStore
{
public:
    /*
     *  Check to see whether we can insert the given item's name
     */
    bool canInsert(const ParentIndex& parent, const std::string& name) const
    {
        // Require that the name is valid and not taken
        return name.size() &&
               names.left.find(std::make_pair(parent, name)) == names.left.end();
    }

    /*
     *  Store an item in the system
     */
    StoredIndex insert(const ParentIndex& parent, const std::string& name,
                       Stored s)
    {
        assert(canInsert(parent, name));

        StoredIndex next = nextIndex();
        storage.insert({next, s});
        names.left.insert({std::make_pair(parent, name), next});

        return next;
    }

    void insert(const ParentIndex& parent, const StoredIndex& i,
                const std::string& name, Stored s)
    {
        assert(!storage.count(i));
        storage.insert({i, s});
        names.left.insert({std::make_pair(parent, name), i});
    }

    /*
     *  Rename a stored item
     */
    void rename(StoredIndex s, const std::string& new_name)
    {
        auto key = names.right.at(s);
        if (key.second != new_name)
        {
            ParentIndex parent = key.first;
            assert(canInsert(parent, new_name));

            names.right.erase(s);
            names.insert({{parent, new_name}, s});
        }
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
     *  Get an item by index (const and non-const versions)
     */
    const Stored& at(StoredIndex i) const
        { return storage.at(i); }
    Stored& at(StoredIndex i)
        { return storage.at(i); }

    /*
     *  Get an item by name and parent
     */
    const Stored& at(const ParentIndex& p, const std::string& name) const
        { return storage.at(indexOf(p, name)); }

    /*
     *  Get an index by name
     *  Fails if no such name exists
     */
    const StoredIndex& indexOf(const ParentIndex& p, const std::string& name) const
        { return names.left.at(std::make_pair(p, name)); }

    /*
     *  Checks whether an item exists, by name
     */
    bool hasItem(const ParentIndex& p, const std::string& name) const
        { return names.left.find(std::make_pair(p, name)) != names.left.end(); }

    /*
     *  Checks to see whether the given item is stored
     */
    bool isValid(const StoredIndex& i) const
        { return storage.find(i) != storage.end(); }

    /*
     *  Get a item's name
     */
    const std::string& nameOf(StoredIndex i) const
        { return names.right.at(i).second; }

    /*
     *  Returns the next available name with the given prefix, constructing
     *  the name as prefix0, prefix1, prefix2, etc.
     */
    std::string nextName(const ParentIndex& p, const std::string prefix) const
    {
        for (unsigned i=0; true; ++i)
        {
            std::stringstream ss;
            ss << prefix << i;
            auto str = ss.str();
            if (canInsert(p, str))
            {
                return str;
            }
        }
    }

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

    /*
     *  Looks up the parent of a particular stored item
     */
    ParentIndex parentOf(const StoredIndex& i) const
    {
        return names.right.at(i).first;
    }

    /*
     *  Erases all stored items
     */
    void reset()
    {
        storage.clear();
        names.clear();
    }

    /*
     *  Returns the next available index
     */
    StoredIndex nextIndex() const
    {
        return storage.size() ? storage.rbegin()->first.i + 1 : 0;
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

}   // namespace Graph
