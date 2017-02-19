#include "graph/root.hpp"

namespace Graph {

Root::Root()
    : deps(*this), interpreter(*this, &deps)
{
    dirty.push({});
}

CellKey Root::toCellKey(const NameKey& k) const
{
    auto sheet = tree.at(k.first.back()).instance()->sheet;
    auto i = tree.indexOf(sheet, k.second);

    assert(tree.at(i).cell());

    return {k.first, CellIndex(i)};
}

NameKey Root::toNameKey(const CellKey& k) const
{
    return {k.first, tree.nameOf(k.second)};
}

CellIndex Root::insertCell(const SheetIndex& sheet, const std::string& name,
                           const std::string& expr)
{
    auto cell = CellIndex(tree.nextIndex());
    insertCell(sheet, cell, name, expr);
    return cell;
}

void Root::insertCell(const SheetIndex& sheet, const CellIndex& cell,
                      const std::string& name, const std::string& expr)
{
    tree.insertCell(sheet, name, expr);
    auto type = interpreter.cellType(expr);
    tree.at(cell).cell()->type = type;

    // Mark the newly-create cell as dirty in all its environments
    for (const auto& e : tree.envsOf(sheet))
    {
        markDirty({e, name});
        changes.push(Response::CellInserted({e, cell}, name, expr));
    }

    if ((type == Cell::INPUT || type == Cell::OUTPUT) &&
        sheet != Tree::ROOT_SHEET)
    {
        // If this could influence sheets that are invoked as functions, then
        // mark all of them for re-evaluation
        auto sheetName = tree.nameOf(sheet);
        for (const auto& e : tree.envsOf(tree.parentOf(sheet)))
        {
            markDirty({e, sheetName});
        }

        // Then, mark input / output changes
        auto R = (type == Cell::INPUT) ? Response::InputErased
                                       : Response::OutputErased;
        for (const auto& e : tree.envsOf(sheet))
        {
            changes.push(R({e, cell}));
        }
    }

    {   // Mark everything that looked at the cell's dummy key as dirty
        std::stringstream ss;
        ss << cell.i;
        markDirty({{}, ss.str()});
    }

    sync();
}

InstanceIndex Root::insertInstance(const SheetIndex& parent,
                                   const std::string& name,
                                   const SheetIndex& target)
{
    auto i = InstanceIndex(tree.nextIndex());
    insertInstance(parent, i, name, target);
    return i;
}

void Root::insertInstance(const SheetIndex& parent, const InstanceIndex& i,
                          const std::string& name, const SheetIndex& target)
{
    tree.insertInstance(parent, i, name, target);

    for (const auto& e : tree.envsOf(parent))
    {
        markDirty({e, name});
        changes.push(Response::InstanceInserted(
                    e, i, name, tree.nameOf(target)));

        // Then, mark all cells as dirty
        for (const auto& c : tree.cellsOf(target))
        {
            auto env = e; // copy
            env.push_back(i);
            env.insert(env.end(), c.first.begin(), c.first.end());

            const auto name = tree.nameOf(c.second);
            const auto expr = tree.at(c.second).cell()->expr;
            markDirty({env, name});
            changes.push(Response::CellInserted({env, c.second}, name, expr));
        }
    }

    // Assign default expressions for inputs, and mark input / output changes
    for (const auto& t : tree.iterItems(target))
    {
        if (auto c = tree.at(t).cell())
        {
            if (c->type == Cell::INPUT)
            {
                const auto expr = interpreter.defaultExpr(c->expr);
                tree.at(i).instance()->inputs[CellIndex(t.i)] = expr;

                for (auto e : tree.envsOf(parent))
                {
                    e.push_back(i);
                    changes.push(Response::InputCreated(
                                {e, CellIndex(t)}, tree.nameOf(t), expr));
                }
            }
            else if (c->type == Cell::OUTPUT)
            {
                for (auto e : tree.envsOf(parent))
                {
                    e.push_back(i);
                    changes.push(Response::OutputCreated(
                                {e, CellIndex(t)}, tree.nameOf(t)));
                }
            }
        }
    }

    sync();
}

void Root::eraseCell(const CellIndex& cell)
{
    auto sheet = tree.parentOf(cell);
    auto name = tree.nameOf(cell);
    auto type = tree.at(cell).cell()->type;

    // Release all interpreter-allocated values
    for (const auto& v : tree.at(cell).cell()->values)
    {
        interpreter.release(v.second.value);
    }

    // Mark this cell as dirty in all its environments, and clear
    // anything that has it marked as downstream
    tree.erase(cell);

    for (const auto& e : tree.envsOf(sheet))
    {
        changes.push(Response::CellErased({e, cell}));
        deps.clear({e, cell});
        markDirty({e, name});
    }

    // Erase all input data associated with this cell
    if (type == Cell::INPUT)
    {
        for (const auto& i : tree.instancesOf(sheet))
        {
            tree.at(i).instance()->inputs.erase(cell);
        }
    }

    // Mark I/O changes
    if (type == Cell::INPUT || type == Cell::OUTPUT)
    {
        auto R = (type == Cell::INPUT) ? Response::InputErased
                                       : Response::OutputErased;
        for (const auto& e : tree.envsOf(sheet))
        {
            changes.push(R({e, cell}));
        }
    }

    {   // Mark everything that looked at the cell's dummy key as dirty
        std::stringstream ss;
        ss << cell.i;
        markDirty({{}, ss.str()});
    }

    sync();
}

void Root::eraseInstance(const InstanceIndex& instance)
{
    // Find all output cells in this instance
    std::set<std::string> outputs;

    auto sheet = tree.at(instance).instance()->sheet;
    auto cells = tree.cellsOf(sheet);
    for (auto i : tree.iterItems(sheet))
    {
        if (auto cell = tree.at(i).cell())
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
        changes.push(Response::InstanceErased(env, instance));

        // Then mark all outputs as dirty, so that anything watching
        // then will also be triggered to re-evaluate itself
        env.push_back(instance);
        for (const auto& o : outputs)
        {
            markDirty({env, o});
        }

        // Finally, clean up all of the cell values so they're not left
        // floating around.
        for (const auto& c : cells)
        {
            // Make a new cell key with the nested environment
            auto env_ = env;
            env_.insert(env_.end(), c.first.begin(), c.first.end());

            auto cell = tree.at(c.second).cell();
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

bool Root::setExpr(const CellIndex& c, const std::string& expr)
{
    auto cell = tree.at(c).cell();
    assert(cell);

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
        for (auto i : tree.instancesOf(tree.parentOf(c)))
        {
            tree.at(i).instance()->inputs[c] = d;
        }

        for (const auto& e : tree.envsOf(tree.parentOf(c)))
        {
            changes.push(Response::InputCreated({e, c}, tree.nameOf(c), d));
        }
    }
    else if (prev_type == Cell::INPUT && cell->type != Cell::INPUT)
    {
        for (auto i : tree.instancesOf(tree.parentOf(c)))
        {
            tree.at(i).instance()->inputs.erase(c);
        }

        for (const auto& e : tree.envsOf(tree.parentOf(c)))
        {
            changes.push(Response::InputErased({e, c}));
        }
    }

    // Transmit output status changes
    if (prev_type == Cell::OUTPUT && cell->type != Cell::OUTPUT)
    {
        for (const auto& e : tree.envsOf(tree.parentOf(c)))
        {
            changes.push(Response::OutputErased({e, c}));
        }
    }
    else if (prev_type != Cell::OUTPUT && cell->type == Cell::OUTPUT)
    {
        for (const auto& e : tree.envsOf(tree.parentOf(c)))
        {
            changes.push(Response::OutputCreated({e, c}, tree.nameOf(c)));
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
        for (const auto& e : tree.envsOf(tree.parentOf(c)))
        {
            for (const auto& d : deps.inverseDeps(toNameKey({e, c})))
            {
                pushDirty(d);
            }
        }
    }

    // Mark all envs containing this cell as dirty
    for (const auto& e : tree.envsOf(tree.parentOf(c)))
    {
        markDirty({e, tree.nameOf(c)});
        changes.push(Response::ExprChanged({e, c}, expr));
    }

    {   // Mark anything that looked for the cell's dummy NameKey as dirty
        // This is how we properly re-evaluate things that call a sheet when
        // a cell internal to that sheet changes.
        std::stringstream ss;
        ss << c.i;
        markDirty({{}, ss.str()});
    }

    sync();

    return true;
}

bool Root::setInput(const InstanceIndex& instance, const CellIndex& cell,
                    const std::string& expr)
{
    assert(tree.at(cell).cell()->type == Cell::INPUT);
    assert(instance.i != 0);

    auto i = tree.at(instance).instance();
    assert(i);
    assert(i->inputs.find(cell) != i->inputs.end());

    if (i->inputs[cell] != expr)
    {
        i->inputs[cell] = expr;
        for (auto env : tree.envsOf(tree.parentOf(instance)))
        {
            env.push_back(instance);
            markDirty(toNameKey({env, cell}));
            changes.push(Response::InputChanged({env, cell}, expr));
        }

        sync();
        return true;
    }
    else
    {
        return false;
    }
}

bool Root::setExprOrInput(const CellKey& cell, const std::string& expr)
{
    if (tree.at(cell.second).cell()->type == Cell::INPUT)
    {
        return setInput(cell.first.back(), cell.second, expr);
    }
    else
    {
        return setExpr(cell.second, expr);
    }
}

void Root::setValue(const CellKey& cell, const Value& v)
{
    auto c = tree.at(cell.second).cell();
    assert(c);

    if (c->values.count(cell.first))
    {
        interpreter.release(c->values.at(cell.first).value);
        c->values.erase(c->values.find(cell.first));
    }
    c->values.insert({cell.first, v});
}

const Value& Root::getValue(const CellKey& cell) const
{
    return tree.at(cell.second).cell()->values.at(cell.first);
}

std::string Root::loadString(const std::string& s)
{
    clear();
    auto err = tree.fromString(s);
    if (err.size())
    {
        clear();
        return err;
    }
    else
    {
        std::set<Cell*> cells;

        for (auto& c : tree.cellsOf(Tree::ROOT_SHEET))
        {
            c.first.push_front(Tree::ROOT_INSTANCE);
            cells.insert(tree.at(c.second).cell());
            pushDirty(c);
        }

        // Parse cells to grab expression type here
        for (auto& c : cells)
        {
            assert(c);
            c->type = interpreter.cellType(c->expr);
        }
        sync();

        return "";
    }
}

bool Root::isItemName(const std::string& name, std::string* err) const
{
    if (name.size() == 0)
    {
        if (err)
        {
            *err = "Name cannot be empty";
        }
        return false;
    }
    else if (!islower(name[0]))
    {
        if (err)
        {
            *err = "Name must begin with a lowercase letter";
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
    else if (interpreter.isReserved(name))
    {
        if (err)
        {
            *err = "Name is reserved by interpreter";
        }
        return false;
    }

    return true;
}

bool Root::checkItemName(const SheetIndex& parent,
                         const std::string& name,
                         std::string* err) const
{
    if (!tree.canInsert(parent, name))
    {
        if (err)
        {
            *err = "Duplicate name";
        }
        return false;
    }

    return isItemName(name, err);;
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

    for (const auto& env : tree.envsOf(tree.parentOf(i)))
    {
        // Mark the old name as dirty
        // (which propagates to anything that looked it up)
        markDirty({env, prev_name});
        changes.push(Response::ItemRenamed(env, i, name));

        // Then, mark anything that looked up the new name as dirty
        for (const auto& d : deps.inverseDeps({env, name}))
        {
            pushDirty(d);
        }
    }
    sync();
}

////////////////////////////////////////////////////////////////////////////////

void Root::clear()
{
    tree.reset();
    deps.reset();
}

std::map<std::string, Value> Root::callSheet(
        const CellKey& caller, const SheetIndex& sheet,
        const std::list<Value> inputs, std::string* err)
{
    auto p = tree.parentOf(caller.second);
    std::map<std::string, Value> out;

    {   // Find and push a dependency on the sheet in its parent environment
        //
        // The sheet will notify this dependency network when I/O changes or
        // when it is erased, triggering re-evaluation of this cell
        auto sheet_parent = tree.parentOf(sheet);
        Env sheet_env = {};
        for (auto& e : caller.first)
        {
            sheet_env.push_back(e);
            if (tree.at(e).instance()->sheet == sheet_parent)
            {
                break;
            }
        }
        deps.insert(caller, {sheet_env, tree.nameOf(sheet)});
    }

    if (!tree.canInsertInstance(p, sheet))
    {
        if (err)
        {
            *err = "Recursive loop when calling sheet";
            return out;
        }
    }

    // Insert a temporary instance anchored to the parent sheet.
    // We insert the instance manually here because calling insertInstance
    // would create O(N^2) operations, as it would insert the instance into
    // every instance of the parent sheet...
    auto instance = tree.insertInstance(p, tree.nextName(p, "i"), sheet);

    // This is our temporary nested execution environment
    auto env = caller.first;
    env.push_back(instance);

    // Prepare to iterate over cells, creating a nested dirty queue
    dirty.push({});
    auto itr = inputs.begin();

    for (const auto& c : tree.cellsOf(sheet))
    {
        // If this is a top-level cell and is an input cell, then inject an
        // value from the input list into the cell for the dummy env
        if (c.first.empty() && tree.at(c.second).cell()->type == Cell::INPUT)
        {
            if (itr == inputs.end())
            {
                if (err)
                {
                    *err = "Too few inputs";
                    dirty.pop();
                    tree.erase(instance);
                    return out;
                }
            }
            assert(itr->valid == true);
            setValue({env, c.second}, *itr++);
        }
        else
        {
            markDirty(toNameKey({env, c.second}));
        }
    }

    // Require that we have exactly the right number of inputs
    if (itr != inputs.end())
    {
        if (err)
        {
            *err = "Too many inputs";
            dirty.pop();
            tree.erase(instance);
            return out;
        }
    }

    // Evaluation!
    sync();
    assert(dirty.top().size() == 0);
    dirty.pop();

    {   // Grab all inputs and outputs and put them in the output map
        std::set<CellIndex> cells;
        for (const auto& c : tree.cellsOf(sheet))
        {
            cells.insert(c.second);
            if (c.first.empty())
            {
                auto cell = tree.at(c.second).cell();
                if (cell->type == Cell::INPUT || cell->type == Cell::OUTPUT)
                {
                    out.insert({tree.nameOf(c.second), cell->values.at(env)});
                }
            }
        }

        // Then, record a dependency all of the cells.  This is a bit of a
        // hack: because dependencies are stored by {Env, Name}, we convert
        // the CellIndex into a string and use it as the name (with an empty
        // Env).  Cells notify this dummy NameKey when they change.
        //
        // TODO: This could be made more efficient by only encoding the
        // upstream deps of output cells in the temporary instance
        for (const auto& c : cells)
        {
            std::stringstream ss;
            ss << c.i;
            deps.insert(caller, {{}, ss.str()});
        }
    }

    // ...and erase the temporary instance
    tree.erase(instance);

    return out;
}

bool Root::isSheetName(const std::string& name, std::string* err) const
{
    if (name.size() == 0)
    {
        if (err)
        {
            *err = "Name cannot be empty";
        }
        return false;
    }
    else if (!isupper(name[0]))
    {
        if (err)
        {
            *err = "Name must begin with a uppercase letter";
        }
        return false;
    }
    return true;
}

bool Root::checkSheetName(const SheetIndex& parent,
                          const std::string& name,
                          std::string* err) const
{
    if (!tree.canInsert(parent, name))
    {
        if (err)
        {
            *err = "Duplicate name";
        }
        return false;
    }

    return isSheetName(name, err);;
}

SheetIndex Root::insertSheet(const SheetIndex& parent, const std::string& name)
{
    auto s = SheetIndex(tree.nextIndex());
    insertSheet(parent, s, name);
    return s;
}

void Root::insertSheet(const SheetIndex& parent, const SheetIndex& sheet,
                       const std::string& name)
{
    assert(canInsertSheet(parent, name));
    tree.insert(parent, sheet, name, Item());

    for (const auto& e : tree.envsOf(parent))
    {
        markDirty({e, name});
        changes.push(Response::SheetCreated(e, sheet, name));
    }
    sync();
}

void Root::renameSheet(const SheetIndex& i, const std::string& name)
{
    tree.rename(i, name);

    for (const auto& e : tree.envsOf(tree.parentOf(i)))
    {
        markDirty({e, name});
        changes.push(Response::SheetRenamed(e, i, name));
    }
}

void Root::eraseSheet(const SheetIndex& s)
{
    auto lock = Lock();

    const auto instances = tree.instancesOf(s);

    // Store parent envs and sheet name so that we can trigger re-evaluation
    // of everything watching the sheet by name
    const auto envs = tree.envsOf(tree.parentOf(s));
    const auto name = tree.nameOf(s);

    for (const auto& i : instances)
    {
        eraseInstance(i);
    }

    for (const auto& c : tree.childrenOf(s))
    {
        if (!tree.isValid(c))
        {
            // Already got erased, e.g. an instance whose sheet was erased
        }
        else if (tree.at(c).cell())
        {
            eraseCell(CellIndex(c));
        }
        else if (tree.at(c).instance())
        {
            eraseInstance(InstanceIndex(c));
        }
        else if (tree.at(c).sheet())
        {
            eraseSheet(SheetIndex(c));
        }
    }

    tree.erase(s);

    //  Re-evaluate anything that was watching the sheet (by name)
    for (const auto& e : envs)
    {
        markDirty({e, name});
        changes.push(Response::SheetErased(e, s));
    }
    // Sync is called on lock destruction
}

void Root::markDirty(const NameKey& k)
{
    // If the key refers to a valid environment and a cell that still
    // exists, then push it to the dirty list directly
    if (k.first.size() && tree.checkEnv(k.first))
    {
        auto sheet = tree.at(k.first.back()).instance()->sheet;
        if (tree.hasItem(sheet, k.second) &&
            tree.at(sheet, k.second).cell())
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
    while (!locked && dirty.top().size())
    {
        const auto k = dirty.top().front();
        auto result = interpreter.eval(k);
        dirty.top().pop_front();

        if (result.value)
        {
            setValue(k, result);
            changes.push(Response::ValueChanged(k, result.str, result.valid));
            for (const auto& d : deps.inverseDeps(toNameKey(k)))
            {
                pushDirty(d);
            }
        }
    }
}

void Root::pushDirty(const CellKey& c)
{
    auto itr = std::find_if(dirty.top().begin(), dirty.top().end(),
        [&](CellKey& o){ return deps.isUpstream(o, c); });

    if (itr == dirty.top().end() || *itr != c)
    {
        dirty.top().insert(itr, c);
    }
}

shared_queue<Response>& Root::run(shared_queue<Command>& input)
{
    future = std::async(std::launch::async, [&]{ _run(input); });
    return changes;
}

void Root::_run(shared_queue<Command>& input)
{
    while (true)
    {
        printf("Waiting...\n");
        input.wait();
        auto cmd = input.pop();
        printf("got cmd\n");

        // Handle the STOP_LOOP meta-command
        if (cmd.op == Command::STOP_LOOP)
        {
            break;
        }
        else
        {
            cmd(*this);
        }
    }
}

}   // namespace Graph
