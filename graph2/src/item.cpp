#include "graph/item.hpp"

#include "graph/cell.hpp"
#include "graph/instance.hpp"

Item::Item(const std::string& e)
    : type(ITEM_CELL)
{
    item.cell = new Cell(e);
}

Item::Item(SheetIndex s)
    : type(ITEM_INSTANCE)
{
    item.instance = new Instance(s);
}

const Instance* Item::instance() const
{
    switch (type)
    {
        case ITEM_CELL: return nullptr;
        case ITEM_INSTANCE: return item.instance;
    }
}

const Cell* Item::cell() const
{
    switch (type)
    {
        case ITEM_CELL: return item.cell;
        case ITEM_INSTANCE: return nullptr;
    }
}

void Item::dealloc()
{
    switch (type)
    {
        case ITEM_CELL:     delete item.cell; break;
        case ITEM_INSTANCE: delete item.instance; break;
    }
}
