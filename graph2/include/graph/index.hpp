#pragma once

/* Use phantom types to make separate item and sheet indices, each containing
 * a single unsigned i */
template <class T>
struct Index
{
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
