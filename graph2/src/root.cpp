#include "graph/root.hpp"

CellKey Root::toCellKey(const NameKey& k) const
{
    auto i = getItem(k.first.back()).instance();
    auto sheet = lib.at(i->sheet);

    return {k.first, sheet.indexOf(k.second)};
}

NameKey Root::toNameKey(const CellKey& k) const
{
    auto sheet = getSheet(getItem(k.first.back()).instance()->sheet);
    return {k.first, sheet.nameOf(k.second)};
}

std::list<Env> Root::envsOf(const SheetIndex& s) const
{
    std::list<std::pair<Env, SheetIndex>> todo;
    std::list<Env> found;

    for (auto i : lib.at(0).iterItems())
    {
        if (auto instance = lib.at(0).at(i).instance())
        {
            todo.push_back({{i}, instance->sheet});
        }
    }

    while (todo.size())
    {
        const auto t = todo.front();
        todo.pop_front();

        const auto env = t.first;
        const auto sheet = t.second;

        if (sheet == s)
        {
            found.push_back(env);
        }
        else
        {
            for (auto i : lib.at(sheet).iterItems())
            {
                auto env_ = env;
                if (auto instance = lib.at(sheet).at(i).instance())
                {
                    env_.push_back(i);
                    todo.push_back({env_, instance->sheet});
                }
            }
        }
    }

    return found;
}

ItemIndex Root::insertCell(const SheetIndex& sheet, const std::string& name,
                           const std::string& expr)
{
    auto cell = tree.insertCell(name, expr, sheet);

    // TODO: Push dirty things here

    return cell;
}

void Root::setExpr(const ItemIndex& i, const std::string& expr)
{
    getMutableItem(i).cell()->expr = expr;
    // TODO: Set cell type
    // TODO: Mark dirty items and sync
}
