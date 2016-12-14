#pragma once

/* Use phantom types to make separate item and sheet indices, each containing
 * a single unsigned i */
template <class T>
struct Index
{
    Index() : i(-1) { /* Default constructor */ }
    Index(unsigned i) : i(i) { /* Nothing to do here */ }
    bool operator<(const Index<T>& other) const {
        return i < other.i;
    }
    bool operator!=(const Index<T>& other) const {
        return i != other.i;
    }
    bool operator==(const Index<T>& other) const {
        return i == other.i;
    }
    unsigned i;
};

struct ItemIndex_ {};
struct CellIndex_ {};
struct InstanceIndex_ {};
struct SheetIndex_ {};

typedef Index<CellIndex_> CellIndex;
typedef Index<InstanceIndex_> InstanceIndex;
typedef Index<SheetIndex_> SheetIndex;

struct ItemIndex : public Index<ItemIndex_>
{
    ItemIndex() : Index(-1) {}
    ItemIndex(unsigned i) : Index(i) {}
    ItemIndex(const CellIndex& j) : Index(j.i) {}
    ItemIndex(const InstanceIndex& j) : Index(j.i) {}
    bool operator<(const ItemIndex& other) const {
        return i < other.i;
    }
    bool operator!=(const ItemIndex& other) const {
        return i != other.i;
    }
    bool operator==(const ItemIndex& other) const {
        return i == other.i;
    }
};
