#include <cassert>

#include "root.hpp"
#include "sheet.hpp"
#include "instance.hpp"
#include "cell.hpp"

namespace Graph
{

Root::Root()
    : sheet(new Sheet()), instance(new Instance(sheet.get())),
      interpreter(s7_init())
{
    /* Nothing to do here */
}

Root::~Root()
{
    free(interpreter);
}

////////////////////////////////////////////////////////////////////////////////

Cell* Root::insertCell(Sheet* sheet, const Name& name, const Expr& expr)
{
    assert(canInsert(sheet, name));

    auto c = new Cell(expr);
    sheet->cells.left[name].reset(c);
    changed(c);

    return c;
}

void Root::editCell(Cell* cell, const Expr& expr)
{
    cell->expr = expr;
    changed(cell);
}

void Root::eraseCell(Cell* cell)
{
    Sheet* parent = parentSheet(cell);
    assert(parent != nullptr);

    // Release Scheme values for GC
    for (auto& v : cell->values)
    {
        s7_gc_unprotect(interpreter, v.second);
    }

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

////////////////////////////////////////////////////////////////////////////////

void Root::rename(Sheet* sheet, const Name& orig, const Name& name)
{
    assert(sheet->cells.left.count(orig) ||
           sheet->instances.left.count(orig));
    assert(canInsert(sheet, name));

    if (sheet->cells.left.count(orig))
    {
        sheet->cells.left[name].swap(sheet->cells.left[orig]);
        sheet->cells.left.erase(name);
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

bool Root::eval(const CellKey& k)
{
    auto& env = k.first;
    auto cell = k.second;

    auto bindings = s7_list(interpreter, 0);
    s7_pointer e = s7_inlet(interpreter, bindings);

    auto value = s7_eval_c_string_with_environment(
            interpreter, cell->expr.c_str(), e);
    if (s7_is_equal(interpreter, value, cell->values[env]))
    {
        return false;
    }
    else
    {
        s7_gc_unprotect(interpreter, cell->values[env]);
        cell->values[env] = value;
        s7_gc_protect(interpreter, cell->values[env]);
        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Root::sync()
{
    if (!locked)
    {
        while (dirty.size())
        {
            const auto& k = dirty.front();
            deps.clear(k);
            if (eval(k))
            {
                for (const auto& d : deps.inverse[toNameKey(k)])
                {
                    dirty.push_back(d);
                }
            }
            dirty.pop_front();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Root::pushDirty(const CellKey& k)
{
    auto itr = dirty.begin();
    while(itr != dirty.end())
    {
        if (deps.upstream[k].count(*itr))
        {
            break;
        }
        ++itr;
    }
    dirty.insert(itr, k);
}

void Root::markDirty(const NameKey& k)
{
    auto& map = k.first.back()->sheet->cells.right;
    auto c = map.find(k.second);

    // If this cell still exists, then push it to the dirty list
    if (c != map.end())
    {
        pushDirty({k.first, c->first.get()});
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
