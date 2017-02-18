#pragma once

#include <string>
#include "graph/types/index.hpp"

namespace Graph {

struct Instance;
struct Cell;

class Item
{
public:
    explicit Item(const std::string& e);
    explicit Item(SheetIndex sheet);
    explicit Item();

    void dealloc();

    const Instance* instance() const;
    Instance* instance();
    const Cell* cell() const;
    Cell* cell();
    bool sheet() const;

    enum Type {ITEM_CELL, ITEM_INSTANCE, ITEM_SHEET};
private:
    const Type type;
    union {
        Cell* cell;
        Instance* instance;
    } item;
};

}   // namespace Graph
