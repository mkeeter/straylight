#include "graph/tree.hpp"
#include "graph/instance.hpp"

Tree::Tree()
{
    SheetIndex default_sheet(0);
    storage.insert({0, default_sheet});
}

Tree::~Tree()
{
    for (auto& i : storage)
    {
        i.second.dealloc();
    }
}

/*
 *  Insert a new cell with the given expression
 */
CellIndex Tree::insertCell(const SheetIndex& parent, const std::string& name,
                           const std::string& expr)
{
    auto c = insert(parent, name, Item(expr));
    order[parent].push_back(c);
    return CellIndex(c.i); // manual conversion!
}

/*
 *  Insert a new instance of the given sheet
 */
InstanceIndex Tree::insertInstance(const SheetIndex& parent,
                                   const std::string& name,
                                   const SheetIndex& target)
{
    auto i = insert(parent, name, Item(target));
    order[parent].push_back(i);
    return InstanceIndex(i.i);
}

bool Tree::canInsertInstance(const SheetIndex& parent, const SheetIndex& target,
                             const std::string& name) const
{
    // TODO: check for recursion here
    (void)target;
    return names.left.find(std::make_pair(parent, name)) == names.left.end();
}

bool Tree::canRename(const ItemIndex& item, const std::string& new_name) const
{
    auto k = names.right.at(item);
    return
        k.second == new_name ||
        names.left.find(std::make_pair(k.first, new_name)) == names.left.end();
}

const std::list<ItemIndex>& Tree::iterItems(const SheetIndex& parent) const
{
    const static std::list<ItemIndex> empty;
    return order.count(parent) ? order.at(parent) : empty;
}

std::list<Env> Tree::envsOf(const SheetIndex& s) const
{
    std::list<std::pair<Env, SheetIndex>> todo = {{{0}, 0}};
    std::list<Env> found;

    while (todo.size())
    {
        const auto t = todo.front();

        const auto env = t.first;
        const auto sheet = t.second;

        if (sheet == s)
        {
            found.push_back(env);
        }
        else
        {
            for (auto i : iterItems(sheet))
            {
                auto env_ = env;
                if (auto instance = at(i).instance())
                {
                    env_.push_back(InstanceIndex(i.i));
                    todo.push_back({env_, instance->sheet});
                }
            }
        }

        todo.pop_front();
    }

    return found;
}

std::list<ItemIndex> Tree::instancesOf(const SheetIndex& s) const
{
    std::list<ItemIndex> out;
    for (const auto& i : storage)
    {
        if (i.second.instance() && i.second.instance()->sheet == s)
        {
            out.push_back(i.first);
        }
    }
    return out;
}

void Tree::erase(const ItemIndex& i)
{
    auto sheet = parentOf(i);
    KeyNameStore::erase(i);
    order[sheet].remove(i);
}

std::list<CellKey> Tree::cellsOf(const SheetIndex& s) const
{
    std::list<CellKey> out;
    for (auto i : iterItems(s))
    {
        if (at(i).cell())
        {
            out.push_back({{}, CellIndex(i.i)});
        }
        else if (auto n = at(i).instance())
        {
            for (auto k : cellsOf(n->sheet))
            {
                k.first.push_front(InstanceIndex(i.i));
                out.push_back(k);
            }
        }
    }
    return out;
}
