#pragma once

#include <cstdint>

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

struct ItemId : public Id<ItemId_>
{
    ItemId() : Id(-1) {}
    ItemId(unsigned i) : Id(i) {}
    ItemId(const CellId& j) : Id(j.i) {}
    ItemId(const InstanceId& j) : Id(j.i) {}
    ItemId(const SheetId& j) : Id(j.i) {}

    bool operator<(const ItemId& other) const {
        return i < other.i;
    }
    bool operator!=(const ItemId& other) const {
        return i != other.i;
    }
    bool operator==(const ItemId& other) const {
        return i == other.i;
    }
};

}   // namespace Graph

