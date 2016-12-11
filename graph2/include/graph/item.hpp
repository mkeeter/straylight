#pragma once

#include <string>
#include "graph/index.hpp"

struct Instance;
struct Cell;

class Item
{
public:
    Item(const std::string& e);
    Item(SheetIndex sheet);

    void dealloc();

    const Instance* instance() const;
    const Cell* cell() const;

    enum Type {ITEM_CELL, ITEM_INSTANCE};
private:
    const Type type;
    union {
        Cell* cell;
        Instance* instance;
    } item;
};
