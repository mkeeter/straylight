#pragma once

#include <cstdint>
#include <map>
#include <list>

namespace Graph {

/* Use phantom types to make separate item and sheet ids,
 * each containing a single unsigned i */
template <class T>
struct Id
{
    Id() : i(-1) { /* Default constructor */ }
    Id(unsigned i) : i(i) { /* Nothing to do here */ }

    template <class M>
    explicit Id(const Id<M>& other) : i(other.i) { /* Nothing to do here */ }

    bool operator<(const Id<T>& other) const {
        return i < other.i;
    }
    bool operator!=(const Id<T>& other) const {
        return i != other.i;
    }
    bool operator==(const Id<T>& other) const {
        return i == other.i;
    }
    uint32_t i;
};

struct ItemId_ {};
struct CellId_ {};
struct InstanceId_ {};
struct SheetId_ {};

typedef Id<CellId_> CellId;
typedef Id<InstanceId_> InstanceId;
typedef Id<SheetId_> SheetId;

/*
 *  An IdMap owns a set of objects with unique Ids.
 *
 *  The Ids are constructed with the lowest 'shift' bits filled with 'mask';
 *  they count upwards from that point.
 *
 *  I must be an Id type.
 *  T can be any type.
 */
template <typename I, typename T, unsigned mask, unsigned shift>
class IdMap
{
public:
    I insert(T* t) {
        auto i = next();
        map.emplace(i, std::unique_ptr<T>(t));
        return i;
    }

    std::unique_ptr<T>& at(const I& i) { return map.at(i); }
    const std::unique_ptr<T>& at(const I& i) const { return map.at(i); }

    void erase(const I& i) {
        auto itr = map.find(i);
        if (itr != map.end())
        {
            unused.push_back(itr->first);
            map.erase(itr);
        }
    }
protected:
    I next() const {
        if (!unused.empty())
        {
            auto i = unused.front();
            unused.pop_front();
            return i;
        }
        else if (map.empty())
        {
            return mask;
        }
        else
        {
            return map.rbegin()->first.i + (1 << shift);
        }
    }
    std::map<I, std::unique_ptr<T>> map;
    mutable std::list<I> unused;
};

}   // namespace Graph

