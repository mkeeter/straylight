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

    // Descend into the graph!
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
