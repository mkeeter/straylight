#include "graph/item.hpp"

#include "graph/cell.hpp"
#include "graph/instance.hpp"

namespace Graph {

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

Item::Item()
    : type(ITEM_SHEET)
{
    // Nothing to do here
}

const Instance* Item::instance() const
{
    switch (type)
    {
        case ITEM_SHEET:    // fallthrough
        case ITEM_CELL:     return nullptr;
        case ITEM_INSTANCE: return item.instance;
    }
}

Instance* Item::instance()
{
    switch (type)
    {
        case ITEM_SHEET:    // fallthrough
        case ITEM_CELL:     return nullptr;
        case ITEM_INSTANCE: return item.instance;
    }
}

const Cell* Item::cell() const
{
    switch (type)
    {
        case ITEM_CELL:     return item.cell;
        case ITEM_SHEET:    // fallthrough
        case ITEM_INSTANCE: return nullptr;
    }
}

Cell* Item::cell()
{
    switch (type)
    {
        case ITEM_CELL:     return item.cell;
        case ITEM_SHEET:    // fallthrough
        case ITEM_INSTANCE: return nullptr;
    }
}

bool Item::sheet() const
{
    return type == ITEM_SHEET;
}

void Item::dealloc()
{
    switch (type)
    {
        case ITEM_CELL:     delete item.cell; break;
        case ITEM_INSTANCE: delete item.instance; break;
        case ITEM_SHEET:    break;
    }
}

}   // namespace Graph
