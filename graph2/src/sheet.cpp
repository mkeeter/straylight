#include "graph/sheet.hpp"
#include "graph/item.hpp"
#include "graph/cell.hpp"
#include "graph/instance.hpp"

Sheet::~Sheet()
{
    for (auto& i : items)
    {
        i.second.dealloc();
    }
}

bool Sheet::canInsert(std::string name) const
{
    return name.size() &&
           names.left.find(name) == names.left.end();
}

ItemIndex Sheet::nextIndex() const
{
    return {items.size() ? items.rbegin()->first.i + 1 : 0};
}

ItemIndex Sheet::insertCell(std::string name, std::string expr)
{
    ItemIndex out = nextIndex();

    items.insert({out, Item(expr)});
    names.left.insert({name, out});
    order.push_back(out);

    return out;
}

ItemIndex Sheet::insertInstance(std::string name, SheetIndex sheet)
{
    ItemIndex out = nextIndex();

    items.insert({out, Item(sheet)});
    names.left.insert({name, out});
    order.push_back(out);

    return out;
}

void Sheet::rename(ItemIndex item, std::string new_name)
{
    auto prev = names.right.at(item);
    assert(canInsert(new_name));

    names.left.erase(prev);
    names.insert({new_name, item});
}

void Sheet::erase(ItemIndex item)
{
    items.erase(item);
    names.right.erase(item);
    order.remove(item);
}
