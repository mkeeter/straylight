#include "graph/root.hpp"

CellKey Root::toCellKey(const NameKey& k) const
{
    auto i = getItem(k.first.back()).instance();

    return {k.first, tree.indexOf(k.second, i->sheet)};
}

NameKey Root::toNameKey(const CellKey& k) const
{
    return {k.first, tree.nameOf(k.second)};
}

ItemIndex Root::insertCell(const SheetIndex& sheet, const std::string& name,
                           const std::string& expr)
{
    auto cell = tree.insertCell(name, expr, sheet);

    for (const auto& e : envsOf(sheet))
    {
        markDirty({e, name});
    }
    sync();

    return cell;
}

void Root::setExpr(const ItemIndex& i, const std::string& expr)
{
    getMutableItem(i).cell()->expr = expr;
    // TODO: Set cell type
    // TODO: mess with input expr if it has changed
    for (const auto& e : envsOf(tree.parentOf(i)))
    {
        markDirty({e, nameOf(i)});
    }
    sync();
}

void Root::setValue(const CellKey& cell, const Value& v)
{
    auto c = getMutableItem(cell.second).cell();
    if (c->values.count(cell.first))
    {
        interpreter.release(c->values.at(cell.first).value);
        c->values.erase(c->values.find(cell.first));
    }
    c->values.insert({cell.first, v});
}

void Root::markDirty(const NameKey& k)
{
    // If this cell still exists, then push it to the dirty list
    auto sheet = getItem(k.first.back()).instance()->sheet;
    if (hasItem(sheet, k.second))
    {
        pushDirty(toCellKey(k));
    }
    else
    {
        // Otherwise push everything downstream of it to the list
        for (auto& i : deps.inverseDeps(k))
        {
            pushDirty(i);
        }
    }
}

void Root::sync()
{
    while (!locked && dirty.size())
    {
        dirty.pop_front();
        // TODO
    }
}

void Root::pushDirty(const CellKey& c)
{
    auto itr = std::find_if(dirty.begin(), dirty.end(),
        [&](CellKey& o){ return deps.isUpstream(o, c); });

    if (itr == dirty.end() || *itr != c)
    {
        dirty.insert(itr, c);
    }
}
