#include <cassert>

#include "root.hpp"
#include "sheet.hpp"
#include "instance.hpp"
#include "cell.hpp"

namespace Graph
{

Root::Root()
    : sheet(new Sheet()), instance(new Instance(sheet.get())),
      interpreter(s7_init()),
      check_upstream(s7_make_function(
                interpreter, "check-upstream", Root::_check_upstream,
                3, /* required args */
                0, /* optional args */
                false, /* rest args */
                "(check-upstream deps target looker)")),
      value_thunk_factory(s7_eval_c_string(interpreter,
            "(lambda (deps target looker check-upstream value)"
            "   (lambda ()"
            "   (let ((res (check-upstream deps target looker)))"
            "       (cond ((=  1 res) (error 'circular-lookup))"
            "             ((= -1 res) (error 'no-such-value))"
            "             (else value)))))"))
{
    // Protect all of our functions from the garbage collector
    s7_gc_protect(interpreter, check_upstream);
    s7_gc_protect(interpreter, value_thunk_factory);
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

bool Root::eval(const CellKey& k)
{
    auto env = k.first;
    auto cell = k.second;

    auto deps_ptr = s7_make_c_pointer(interpreter, &deps);
    auto looker = toList(k);

    // Input cells are evaluated in their parent's environment and with an
    // input string from their containing Instance
    std::string expr;
    if (cell->isInput())
    {
        expr = env.back()->inputs[cell];
        env.pop_back();
    }
    else
    {
        expr = cell->expr;
    }

    s7_pointer value;
    if (!env.size())
    {
        value = s7_eval_c_string(interpreter,
                "(error 'input-at-top-level)");
    }
    else
    {
        auto bindings = s7_list(interpreter, 0);
        for (auto& c : env.back()->sheet->cells.left)
        {
            // Construct a lookup thunk using value_thunk_factory
            auto target = toList({env, c.second});
            auto args = s7_list(interpreter, 5, deps_ptr, target, looker,
                                check_upstream,
                                c.second->values.count(env)
                                    ? c.second->values.at(env)
                                    : s7_nil(interpreter));
            auto thunk = s7_call(interpreter, value_thunk_factory, args);

            // Then push it to the list
            bindings = s7_cons(interpreter, thunk, bindings);

            // Prepend the symbol name to the bindings list
            bindings = s7_cons(interpreter,
                    s7_make_symbol(interpreter, c.first.c_str()),
                    bindings);
        }
        for (auto& i : env.back()->sheet->instances)
        {
            // Create a temporary environment inside the instance
            auto env_ = env;
            env_.push_back(i.second.get());

            s7_pointer callback=nullptr;
            for (auto o : i.second->sheet->outputs())
            {
                if (deps.upstream[k].count({env_, o}))
                {
                    // Insert callback that calls an error
                }
                else
                {
                    // Insert callback beep boop
                }
            }
            bindings = s7_cons(interpreter, callback, bindings);

            // Push the symbol name to the bindings list
            bindings = s7_cons(interpreter,
                    s7_make_symbol(interpreter, i.first.c_str()),
                    bindings);

        }

        // Run the evaluation and get out a value
        expr = "(begin " + expr + ")";
        value = s7_eval_c_string_with_environment(
                interpreter, expr.c_str(),
                s7_inlet(interpreter, bindings));
    }

    // If the value is the same, return false
    if (cell->values.count(env) &&
        s7_is_equal(interpreter, value, cell->values[env]))
    {
        return false;
    }
    // Otherwise, swap out the value and return true
    else
    {
        if (cell->values.count(env))
        {
            s7_gc_unprotect(interpreter, cell->values[env]);
        }
        cell->values[env] = value;
        s7_gc_protect(interpreter, cell->values[env]);

        // Also get a string representation out
        auto str_ptr = s7_object_to_c_string(interpreter, value);
        cell->strs[env] = std::string(str_ptr);
        free(str_ptr);

        return true;
    }
}

Value Root::toList(const CellKey& k)
{
    auto out = s7_list(interpreter, 0);
    for (auto& i : k.first)
    {
        out = s7_cons(interpreter, s7_make_c_pointer(interpreter, i), out);
    }
    out = s7_cons(interpreter, s7_make_c_pointer(interpreter, k.second), out);
    return out;
}

CellKey Root::fromList(Value v)
{
    CellKey out;
    out.second = static_cast<Cell*>(s7_c_pointer(s7_car(v)));
    v = s7_cdr(v);

    // Because of the list's order, we fill the key from back to front
    out.first.resize(s7_list_length(interpreter, v));
    for (auto itr = out.first.rbegin(); itr != out.first.rend();
         ++itr, v = s7_cdr(v))
    {
        *itr = static_cast<Instance*>(s7_c_pointer(s7_car(v)));
    }

    return out;
}

s7_pointer Root::_check_upstream(s7_scheme* interpreter, s7_pointer args)
{
    auto& root = *static_cast<Root*>(s7_c_pointer(s7_car(args)));
    auto lookee = root.fromList(s7_cadr(args));
    auto looker = root.fromList(s7_caddr(args));

    int out = 0;
    if (lookee.second->values.count(looker.first))
    {
        out = -1;
    }
    else if (root.deps.insert(looker, toNameKey(lookee)))
    {
        out = 1;
    }
    return s7_make_integer(interpreter, out);
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
    dirty.insert(std::find_if(dirty.begin(), dirty.end(),
            [&](CellKey& o){ return deps.upstream[k].count(o) != 0; }),
            k);
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
