#include "graph/root.hpp"

CellKey Root::toCellKey(const NameKey& k) const
{
    auto sheet = getItem(k.first.back()).instance()->sheet;
    auto i = tree.indexOf(sheet, k.second);

    assert(getItem(i).cell());

    return {k.first, CellIndex(i.i)};
}

NameKey Root::toNameKey(const CellKey& k) const
{
    return {k.first, tree.nameOf(k.second)};
}

CellIndex Root::insertCell(const SheetIndex& sheet, const std::string& name,
                           const std::string& expr)
{
    auto cell = tree.insertCell(sheet, name, expr);
    getMutableItem(cell).cell()->type = interpreter.cellType(expr);

    for (const auto& e : tree.envsOf(sheet))
    {
        markDirty({e, name});
    }
    sync();

    return cell;
}

InstanceIndex Root::insertInstance(const SheetIndex& parent,
                                   const std::string& name,
                                   const SheetIndex& target)
{
    auto i = tree.insertInstance(parent, name, target);

    for (const auto& e : tree.envsOf(parent))
    {
        markDirty({e, name});

        // Then, mark all cells as dirty
        for (const auto& c : tree.cellsOf(target))
        {
            auto env = e; // copy
            env.push_back(i);
            env.insert(e.end(), c.first.begin(), c.first.end());
            markDirty({env, nameOf(c.second)});
        }
    }

    // Assign default expressions for inputs
    for (const auto& t : iterItems(target))
    {
        if (auto c = getItem(t).cell())
        {
            if (c->type == Cell::INPUT)
            {
                getMutableItem(i).instance()->inputs[t] =
                    interpreter.defaultExpr(c->expr);
            }
        }
    }

    sync();
    return i;
}

void Root::eraseCell(const CellIndex& cell)
{
    auto sheet = tree.parentOf(cell);
    auto name = tree.nameOf(cell);
    bool was_input = getItem(cell).cell()->type == Cell::INPUT;

    // Release all interpreter-allocated values
    for (const auto& v : getItem(cell).cell()->values)
    {
        interpreter.release(v.second.value);
    }

    // Mark this cell as dirty in all its environments, and clear
    // anything that has it marked as downstream
    tree.erase(cell);
    for (const auto& e : tree.envsOf(sheet))
    {
        deps.clear({e, cell});
        markDirty({e, name});
    }

    // Erase all input data associated with this cell
    if (was_input)
    {
        for (const auto& i : tree.instancesOf(sheet))
        {
            getMutableItem(i).instance()->inputs.erase(cell);
        }
    }
    sync();
}

void Root::eraseInstance(const InstanceIndex& instance)
{
    // Find all output cells in this instance
    std::set<std::string> outputs;

    auto sheet = getItem(instance).instance()->sheet;
    auto cells = tree.cellsOf(sheet);
    for (auto i : iterItems(sheet))
    {
        if (auto cell = getItem(i).cell())
        {
            if (cell->type == Cell::OUTPUT)
            {
                outputs.insert(tree.nameOf(i));
            }
        }
    }

    const std::string name = tree.nameOf(instance);
    const auto parent = tree.parentOf(instance);
    tree.erase(instance); // Bye!
    for (auto env : tree.envsOf(parent))
    {
        // Mark the instance itself as dirty
        markDirty({env, name});

        // Then mark all outputs as dirty
        env.push_back(instance);
        for (const auto& o : outputs)
        {
            markDirty({env, o});
        }
        for (const auto& c : cells)
        {
            // Make a new cell key with the nested environment
            auto env_ = env;
            env_.insert(env_.end(), c.first.begin(), c.first.end());

            auto cell = getMutableItem(c.second).cell();
            assert(cell != nullptr);

            // Release the allocated value, then erase by key
            interpreter.release(cell->values.at(env_).value);
            cell->values.erase(env_);

            // Then clear dependencies for this cell, so that we don't
            // end up trying to evaluate it
            deps.clear({env_, c.second});
        }
    }
    sync();
}

bool Root::setExpr(const CellIndex& i, const std::string& expr)
{
    auto cell = getMutableItem(i).cell();

    // Early exit if this is a no-op
    if (cell->expr == expr)
    {
        return false;
    }

    cell->expr = expr;

    // Update cell type, saving previous type
    auto prev_type = cell->type;
    cell->type = interpreter.cellType(expr);

    // Handle default expressions for input cells
    if (prev_type != Cell::INPUT && cell->type == Cell::INPUT)
    {
        auto d = interpreter.defaultExpr(expr);
        for (auto instance : tree.instancesOf(tree.parentOf(i)))
        {
            getMutableItem(instance).instance()->inputs[i] = d;
        }
    }
    else if (prev_type == Cell::INPUT && cell->type != Cell::INPUT)
    {
        for (auto instance : tree.instancesOf(tree.parentOf(i)))
        {
            getMutableItem(instance).instance()->inputs.erase(i);
        }
    }

    // Force all dependencies to be recalculated if we've changed
    // from non-output to output or vice versa.
    //
    // This kind of change could preserve the cell's value but change
    // dependencies, which are either allowed or no longer allowed to
    // look at its value.
    if ((prev_type == Cell::OUTPUT) ^ (cell->type == Cell::OUTPUT))
    {
        for (const auto& e : tree.envsOf(tree.parentOf(i)))
        {
            for (const auto& d : deps.inverseDeps(toNameKey({e, i})))
            {
                pushDirty(d);
            }
        }
    }

    // Mark all envs containing this cell as dirty
    for (const auto& e : tree.envsOf(tree.parentOf(i)))
    {
        markDirty({e, tree.nameOf(i)});
    }
    sync();

    return true;
}

void Root::setInput(const InstanceIndex& instance, const CellIndex& cell,
                    const std::string& expr)
{
    assert(getItem(cell).cell()->type == Cell::INPUT);
    assert(instance.i != 0);

    auto i = getMutableItem(instance).instance();
    assert(i->inputs.find(cell) != i->inputs.end());

    i->inputs[cell] = expr;
    for (auto env : tree.envsOf(tree.parentOf(instance)))
    {
        env.push_back(instance);
        markDirty(toNameKey({env, cell}));
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

const Value& Root::getValue(const CellKey& cell) const
{
    return getItem(cell.second).cell()->values.at(cell.first);
}

bool Root::checkName(const SheetIndex& parent,
                     const std::string& name,
                     std::string* err) const
{
    if (name.size() == 0)
    {
        if (err)
        {
            *err = "Name cannot be empty";
        }
        return false;
    }
    else if (!tree.canInsert(parent, name))
    {
        if (err)
        {
            *err = "Duplicate name";
        }
        return false;
    }
    else if (!interpreter.nameValid(name))
    {
        if (err)
        {
            *err = "Invalid name";
        }
        return false;
    }

    return true;
}

void Root::renameItem(const ItemIndex& i, const std::string& name)
{
    auto prev_name = tree.nameOf(i);

    // Skip renaming if it's a no-op
    if (prev_name == name)
    {
        return;
    }

    tree.rename(i, name);

    for (const auto& env : tree.envsOf(parentSheet(i)))
    {
        // Mark the old name as dirty
        // (which propagates to anything that looked it up)
        markDirty({env, prev_name});

        // Then, mark anything that looked up the new name as dirty
        for (const auto& d : deps.inverseDeps({env, name}))
        {
            pushDirty(d);
        }
    }
    sync();
}

void Root::serialize(TreeSerializer* s) const
{
    s->instance(0, "");
    serialize(s, {0});
}

void Root::serialize(TreeSerializer* s, const Env& env) const
{
    auto sheet = getItem(env.back()).instance()->sheet;

    if (s->push())
    {
        for (auto i : iterItems(sheet))
        {
            const auto& name = tree.nameOf(i);
            const auto& item = getItem(i);

            if (auto c = item.cell())
            {
                const auto& value = c->values.at(env);
                s->cell(CellIndex(i.i), name, c->expr, c->type,
                        value.valid, value.str);
            }
            else if (auto n = item.instance())
            {
                InstanceIndex index(i.i);
                s->instance(index, name);

                // TODO: draw inputs and outputs here
                auto env_ = env;
                env_.push_back(index);

                for (auto item: iterItems(n->sheet))
                {
                    if (auto c = getItem(item).cell())
                    {
                        const auto& v = c->values.at(env_);
                        if (c->type == Cell::INPUT)
                        {
                            s->input(CellIndex(item.i), n->inputs.at(item),
                                     nameOf(item), v.valid, v.str);
                        }
                        else if (c->type == Cell::OUTPUT)
                        {
                            s->output(CellIndex(item.i), nameOf(item),
                                      v.valid, v.str);
                        }
                    }
                }
                serialize(s, env_);
            }
        }
    }
    s->pop();
}

bool Root::checkEnv(const Env& env) const
{
    for (const auto& i : env)
    {
        if (!tree.isValid(i) ||
            getItem(i).instance() == nullptr)
        {
            return false;
        }
    }
    return true;
}

void Root::markDirty(const NameKey& k)
{
    // If the key refers to a valid environment and a cell that still
    // exists, then push it to the dirty list directly
    if (checkEnv(k.first))
    {
        auto sheet = getItem(k.first.back()).instance()->sheet;
        if (hasItem(sheet, k.second) &&
            getItem(sheet, k.second).cell())
        {
            pushDirty(toCellKey(k));
            return;
        }
    }

    // Otherwise push everything downstream of it to the list
    for (auto& i : deps.inverseDeps(k))
    {
        pushDirty(i);
    }
}

void Root::sync()
{
    while (!locked && dirty.size())
    {
        const auto k = dirty.front();
        auto result = interpreter.eval(k);
        dirty.pop_front();

        if (result.value)
        {
            setValue(k, result);
            for (const auto& d : deps.inverseDeps(toNameKey(k)))
            {
                pushDirty(d);
            }
        }
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
