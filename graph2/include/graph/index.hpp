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
struct SheetIndex_ {};

typedef Index<ItemIndex_> ItemIndex;
typedef Index<SheetIndex_> SheetIndex;
