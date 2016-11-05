#include <cassert>

#include "root.hpp"
#include "sheet.hpp"
#include "instance.hpp"
#include "cell.hpp"

namespace Graph
{

Root::Root()
    : sheet(new Sheet(nullptr)), instance(new Instance(sheet.get(), nullptr))
{
    // Nothing to do here
}

////////////////////////////////////////////////////////////////////////////////

Cell* Root::insertCell(Sheet* sheet, const Name& name, const Expr& expr)
{
    assert(canInsert(sheet, name));

    auto c = new Cell(expr, sheet);
    c->type = interpreter.cellType(c);
    sheet->cells.insert({name, c});
    sheet->order.push_back(c);
    changed(c);

    return c;
}

void Root::editCell(Cell* cell, const Expr& expr)
{
    cell->expr = expr;
    cell->type = interpreter.cellType(cell);

    // If this changed inputs or outputs, do something here!
    changed(cell);
}

void Root::eraseCell(Cell* cell)
{
    Sheet* parent = cell->parent;
    assert(parent != nullptr);

    // Release Scheme values for GC
    interpreter.release(cell);

    auto name = parent->cells.right.at(cell);
    parent->cells.right.erase(cell);
    parent->order.erase(std::find(parent->order.begin(),
                                  parent->order.end(), cell));
    delete cell;

    // Mark that this cell has changed (by name, rather than pointer, since
    // we just deleted the cell itself).  This will cause all downstream cells
    // to re-evaluate themselves.
    changed(parent, name);
}

void Root::eraseInstance(Instance* i)
{
    Sheet* parent = i->parent;
    assert(parent != nullptr);

    auto name = parent->instances.right.at(i);
    parent->instances.right.erase(i);
    parent->order.erase(std::find(parent->order.begin(),
                                  parent->order.end(), i));
    delete i;

    // Mark that this cell has changed (by name, rather than pointer, since
    // we just deleted the cell itself).  This will cause all downstream cells
    // to re-evaluate themselves.
    changed(parent, name);
}

////////////////////////////////////////////////////////////////////////////////

Sheet* Root::createSheet(Sheet* sheet, const Name& name)
{
    assert(canCreateSheet(sheet, name));
    auto s = new Sheet(sheet);
    sheet->library.insert({name, s});
    return s;
}

bool Root::canCreateSheet(Sheet* sheet, const Name& name) const
{
    return sheet->library.left.count(name) == 0;
}

void Root::renameSheet(Sheet* sheet, const Name& orig, const Name& name)
{
    auto ptr = sheet->library.left.at(orig);
    sheet->library.left.erase(orig);
    sheet->library.insert({name, ptr});
}

////////////////////////////////////////////////////////////////////////////////

bool Root::canInsertInstance(Sheet* sheet, Sheet* target) const
{
    {   // Make sure that the sheet to be inserted is above the sheet which it
        // will be an instance in.
        bool above = false;
        auto sheet_ = sheet;
        while (sheet_ && !above)
        {
            if (sheet_->library.right.count(target))
            {
                above = true;
            }
            sheet_ = sheet_->parent;
        }
        if (!above)
        {
            return false;
        }
    }

    {   // Then, check for recursive loops from the target to the parent
        std::set<Sheet*> checked = {target};
        std::list<Sheet*> todo = {target};
        while (todo.size())
        {
            auto s = todo.front();
            if (s == sheet)
            {
                return false;
            }

            for (const auto& h : s->instances.left)
            {
                if (checked.count(h.second->sheet) == 0)
                {
                    todo.push_back(h.second->sheet);
                    checked.insert(h.second->sheet);
                }
            }
            todo.pop_front();
        }
    }

    return true;
}

Instance* Root::insertInstance(Sheet* sheet, const Name& name, Sheet* target)
{
    assert(canInsertInstance(sheet, target));
    assert(canInsert(sheet, name));

    auto added = new Instance(target, sheet);

    // Set default expressions for any inputs
    for (const auto& cell : target->cells.left)
    {
        if (cell.second->type == Cell::INPUT)
        {
            added->inputs[cell.second] = interpreter.defaultExpr(cell.second);
        }
    }

    sheet->instances.insert({name, added});
    sheet->order.push_back(added);

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
            for (auto& j : s->instances.left)
            {
                auto i_ = i;
                i_.push_back(j.second);
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
            for (auto& j : s->instances.left)
            {
                // If this is an example of the added instance, then add all
                // of the cells that we found earlier underneath the instance
                if (j.second == added)
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
                    i_.push_back(j.second);
                    todo.push_back(i_);
                }
            }
            todo.pop_front();
        }
    }

    sync();

    return added;
}

bool Root::canInsert(Sheet* const sheet, const Name& name) const
{
    return (sheet->cells.left.count(name) == 0) &&
           (sheet->instances.left.count(name) == 0) &&
           interpreter.nameValid(name);
}

////////////////////////////////////////////////////////////////////////////////

void Root::rename(Sheet* sheet, const Name& orig, const Name& name)
{
    assert(sheet->cells.left.count(orig) ||
           sheet->instances.left.count(orig));
    assert(canInsert(sheet, name));

    // Prevent sync calls until the end of this function
    auto lock = Lock();

    // Call changed now so that anything that looked for the new name will be
    // added to the dirty list; if we called this after the new name was
    // valid, it would re-evaluate the new name and notice that it hadn't
    // changed and thus not mark anything else as changed.
    changed(sheet, name);

    if (sheet->cells.left.count(orig))
    {
        auto ptr = sheet->cells.left.at(orig);
        sheet->cells.left.erase(orig);
        sheet->cells.insert({name, ptr});
    }
    else if (sheet->instances.left.count(orig))
    {
        auto ptr = sheet->instances.left.at(orig);
        sheet->instances.left.erase(orig);
        sheet->instances.insert({name, ptr});
    }

    changed(sheet, orig);
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
        for (const auto& j : s->instances.left)
        {
            auto i_ = i;
            i_.push_back(j.second);
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

        if (s == sheet)
        {
            markDirty({i, name});
        }
        else
        {
            for (const auto& j : s->instances.left)
            {
                auto i_ = i;
                i_.push_back(j.second);
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
