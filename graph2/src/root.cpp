#include "graph/root.hpp"

bool Root::isCell(const NameKey& k) const
{
    auto i = getInstance(k.first);
    auto sheet = lib.at(i->sheet);

    return sheet.hasItem(k.second) &&
           sheet.at(k.second).cell();
}

const Instance* Root::getInstance(const std::list<ItemIndex>& i) const
{
    const Instance* target = instance.get();
    for (auto index : i)
    {
        target = lib.at(target->sheet).at(index).instance();
        assert(target != nullptr);
    }
    return target;
}

CellKey Root::toCellKey(const NameKey& k) const
{
    auto i = getInstance(k.first);
    auto sheet = lib.at(i->sheet);

    return {k.first, sheet.indexOf(k.second)};
}
