#include <cassert>

#include "root.hpp"
#include "sheet.hpp"
#include "instance.hpp"
#include "cell.hpp"

namespace Graph
{

Root::Root()
    : sheet(new Sheet()), instance(new Instance(sheet.get()))
{
    // Nothing to do here
}

////////////////////////////////////////////////////////////////////////////////

Cell* Root::insertCell(Sheet* sheet, const Name& name, const Expr& expr)
{
    assert(canInsert(sheet, name));

    auto c = new Cell(expr);
    sheet->cells.insert({name, c});
    changed(c);

    return c;
}

void Root::editCell(Cell* cell, const Expr& expr)
{
    cell->expr = expr;
    // If this changed inputs or outputs, do something here!
    changed(cell);
}

void Root::eraseCell(Cell* cell)
{
    Sheet* parent = parentSheet(cell);
    assert(parent != nullptr);

    // Release Scheme values for GC
    interpreter.release(cell);

    auto name = parent->cells.right.at(cell);
    parent->cells.right.erase(cell);

    // Mark that this cell has changed (by name, rather than pointer, since
    // we just deleted the cell itself).  This will cause all downstream cells
    // to re-evaluate themselves.
    changed(parent, name);
}

Sheet* Root::parentSheet(Cell* cell) const
{
    std::list<Sheet*> todo = {sheet.get()};
    while (todo.size())
    {
        const auto& s = todo.front();
        if (s->cells.right.count(cell))
        {
            return s;
        }
        for (auto& k : s->library)
        {
            todo.push_back(k.second.get());
        }
        todo.pop_front();
    }
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////

Instance* Root::insertInstance(Sheet* sheet, const Name& name, Sheet* target)
{
    assert(canInsert(sheet, name));

    auto added = new Instance(target);

    // Set default expressions for any inputs
    for (auto cell : target->cells.left)
    {
        if (interpreter.isInput(cell.second))
        {
            added->inputs[cell.second] = interpreter.defaultExpr(cell.second);
        }
    }

    sheet->instances[name].reset(added);
    std::list<NameKey> changed;
    {   // Recursively find all cells in this sheet.
        std::list<Env> todo = {{added}};
        while (todo.size())
        {
            const auto& i = todo.front();
            auto s = i.back()->sheet;
            for (const auto& c : s->cells.left)
            {
                changed.push_back({i, c.first});
            }
            for (auto& j : s->instances)
            {
                auto i_ = i;
                i_.push_back(j.second.get());
                todo.push_back(i_);
            }
            todo.pop_back();
        }
    }

    {   // Then recurse down the rest of the sheet, marking as dirty the
        // product of the changed instances and where they're instantiated.
        std::list<Env> todo = {{instance.get()}};
        while (todo.size())
        {
            const auto& i = todo.front();
            auto s = i.back()->sheet;
            for (auto& j : s->instances)
            {
                // If this is an example of the added instance, then add all
                // of the cells that we found earlier underneath the instance
                if (j.second.get() == added)
                {
                    for (auto k : changed)
                    {
                        k.first.insert(k.first.begin(), i.begin(), i.end());
                        markDirty(k);
                    }
                }
                else
                {
                    auto i_ = i;
                    i_.push_back(j.second.get());
                    todo.push_back(i_);
                }
            }
            todo.pop_back();
        }
    }

    sync();

    return added;
}

bool Root::canInsert(Sheet* const sheet, const Name& name) const
{
    return (sheet->cells.left.count(name) == 0) &&
           (sheet->instances.count(name) == 0);
}

////////////////////////////////////////////////////////////////////////////////

void Root::rename(Sheet* sheet, const Name& orig, const Name& name)
{
    assert(sheet->cells.left.count(orig) ||
           sheet->instances.count(orig));
    assert(canInsert(sheet, name));

    if (sheet->cells.left.count(orig))
    {
        sheet->cells.insert({name, sheet->cells.left.at(orig)});
        sheet->cells.left.erase(orig);
    }
    else if (sheet->instances.count(orig))
    {
        sheet->instances[name].swap(sheet->instances[orig]);
        sheet->instances.erase(name);
    }

    auto lock = Lock();
    changed(sheet, orig);
    changed(sheet, name);
}

////////////////////////////////////////////////////////////////////////////////

void Root::changed(Cell* cell)
{
    std::list<Env> todo = {{instance.get()}};

    // Recurse down the graph, marking every instance of this Cell as dirty
    while (todo.size())
    {
        const auto& i = todo.front();
        auto s = i.back()->sheet;

        if (s->cells.right.count(cell))
        {
            markDirty({i, s->cells.right.at(cell)});
        }
        for (const auto& j : s->instances)
        {
            auto i_ = i;
            i_.push_back(j.second.get());
            todo.push_back(i_);
        }
        todo.pop_front();
    }

    sync();
}

void Root::changed(Sheet* sheet, const Name& name)
{
    std::list<Env> todo = {{instance.get()}};

    // Recurse down the graph, marking every instance of this Cell as dirty
    while (todo.size())
    {
        const auto& i = todo.front();
        auto s = i.back()->sheet;

        if (s == sheet && s->cells.left.count(name))
        {
            markDirty({i, name});
        }
        for (const auto& j : s->instances)
        {
            if (s == sheet && j.first == name)
            {
                markDirty({i, name});
            }
            else
            {
                auto i_ = i;
                i_.push_back(j.second.get());
                todo.push_back(i_);
            }
        }
        todo.pop_front();
    }

    sync();
}

////////////////////////////////////////////////////////////////////////////////

void Root::sync()
{
    if (!locked)
    {
        while (dirty.size())
        {
            const auto k = dirty.front();
            auto result = interpreter.eval(k, &deps);
            dirty.pop_front();

            if (result)
            {
                for (const auto& d : deps.inverse[toNameKey(k)])
                {
                    pushDirty(d);
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Root::pushDirty(const CellKey& k)
{
    auto itr = std::find_if(dirty.begin(), dirty.end(),
        [&](CellKey& o){ return deps.upstream[o].count(k) != 0; });

    if (itr == dirty.end() || *itr != k)
    {
        dirty.insert(itr, k);
    }
}

void Root::markDirty(const NameKey& k)
{
    auto& map = k.first.back()->sheet->cells.left;
    auto c = map.find(k.second);

    // If this cell still exists, then push it to the dirty list
    if (c != map.end())
    {
        pushDirty({k.first, c->second});
    }
    else
    {
        // Otherwise push everything upstream of it to the list
        for (auto& i : deps.inverse[k])
        {
            pushDirty(i);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

}   // namespace Graph
