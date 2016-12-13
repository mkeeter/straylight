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
ItemIndex Tree::insertCell(std::string name, std::string expr,
                           SheetIndex parent)
{
    auto c = insert(name, Item(expr), parent);
    order[parent].push_back(c);
    return c;
}

/*
 *  Insert a new instance of the given sheet
 */
ItemIndex Tree::insertInstance(std::string name, SheetIndex sheet,
                               SheetIndex parent)
{
    auto i = insert(name, Item(sheet), parent);
    order[parent].push_back(i);
    return i;
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
                    env_.push_back(i);
                    todo.push_back({env_, instance->sheet});
                }
            }
        }

        todo.pop_front();
    }

    return found;
}

