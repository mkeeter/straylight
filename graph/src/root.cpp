#include "graph/root.hpp"

namespace Graph {

CellKey Root::toCellKey(const NameKey& k) const
{
    auto sheet = getItem(k.first.back()).instance()->sheet;
    auto i = tree.indexOf(sheet, k.second);

    assert(getItem(i).cell());

    return {k.first, CellIndex(i)};
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

void Root::insertCell(const SheetIndex& sheet, const CellIndex& cell,
                      const std::string& name, const std::string& expr)
{
    tree.insertCell(sheet, cell, name, expr);
    getMutableItem(cell).cell()->type = interpreter.cellType(expr);

    for (const auto& e : tree.envsOf(sheet))
    {
        markDirty({e, name});
    }
    sync();
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
            markDirty({env, itemName(c.second)});
        }
    }

    // Assign default expressions for inputs
    for (const auto& t : iterItems(target))
    {
        if (auto c = getItem(t).cell())
        {
            if (c->type == Cell::INPUT)
            {
                getMutableItem(i).instance()->inputs[CellIndex(t.i)] =
                    interpreter.defaultExpr(c->expr);
            }
        }
    }

    sync();
    return i;
}

void Root::insertInstance(const SheetIndex& parent, const InstanceIndex& i,
                          const std::string& name, const SheetIndex& target)
{
    tree.insertInstance(parent, i, name, target);

    for (const auto& e : tree.envsOf(parent))
    {
        markDirty({e, name});

        // Then, mark all cells as dirty
        for (const auto& c : tree.cellsOf(target))
        {
            auto env = e; // copy
            env.push_back(i);
            env.insert(e.end(), c.first.begin(), c.first.end());
            markDirty({env, itemName(c.second)});
        }
    }

    // Assign default expressions for inputs
    for (const auto& t : iterItems(target))
    {
        if (auto c = getItem(t).cell())
        {
            if (c->type == Cell::INPUT)
            {
                getMutableItem(i).instance()->inputs[CellIndex(t.i)] =
                    interpreter.defaultExpr(c->expr);
            }
        }
    }

    sync();
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

bool Root::setInput(const InstanceIndex& instance, const CellIndex& cell,
                    const std::string& expr)
{
    assert(getItem(cell).cell()->type == Cell::INPUT);
    assert(instance.i != 0);

    auto i = getMutableItem(instance).instance();
    assert(i->inputs.find(cell) != i->inputs.end());

    if (i->inputs[cell] != expr)
    {
        i->inputs[cell] = expr;
        for (auto env : tree.envsOf(tree.parentOf(instance)))
        {
            env.push_back(instance);
            markDirty(toNameKey({env, cell}));
        }

        sync();
        return true;
    }
    else
    {
        return false;
    }
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

bool Root::checkItemName(const SheetIndex& parent,
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
    else if (!islower(name[0]))
    {
        if (err)
        {
            *err = "Name must begin with a lowercase letter";
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

void Root::renameItem(const ItemIndex& i, const std::string& name)
{
    auto prev_name = tree.nameOf(i);

    // Skip renaming if it's a no-op
    if (prev_name == name)
    {
        return;
    }

    tree.rename(i, name);

    for (const auto& env : tree.envsOf(itemParent(i)))
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
    s->instance(0, "", "");
    serialize(s, {0});
}

void Root::serialize(TreeSerializer* s, const Env& env) const
{
    auto instance = env.back();
    auto sheet = getItem(instance).instance()->sheet;

    if (s->push(instance.i, instance.i ? tree.nameOf(instance) : "",
                sheet.i ? lib.nameOf(sheet) : ""))
    {
        for (auto i : iterItems(sheet))
        {
            const auto& name = tree.nameOf(i);
            const auto& item = getItem(i);

            if (auto c = item.cell())
            {
                const auto& value = c->values.at(env);
                s->cell(CellIndex(i), name, c->expr, c->type,
                        value.valid, value.str, value.value);
            }
            else if (auto n = item.instance())
            {
                InstanceIndex index(i.i);
                s->instance(index, name, lib.nameOf(n->sheet));

                // TODO: draw inputs and outputs here
                auto env_ = env;
                env_.push_back(index);

                for (auto item : iterItems(n->sheet))
                {
                    if (auto c = getItem(item).cell())
                    {
                        const auto& v = c->values.at(env_);
                        if (c->type == Cell::INPUT)
                        {
                            s->input(CellIndex(item), itemName(item),
                                     n->inputs.at(CellIndex(item)),
                                     v.valid, v.str);
                        }
                        else if (c->type == Cell::OUTPUT)
                        {
                            s->output(CellIndex(item), itemName(item),
                                      v.valid, v.str);
                        }
                    }
                }
                serialize(s, env_);
            }
        }

        {   // Pass all sheets through to the serializer
            std::set<SheetIndex> sheets_above;
            std::set<SheetIndex> sheets_direct;

            // Walk through parents, accumulating sheets
            for (const auto& v : env)
            {
                auto es = lib.childrenOf(getItem(v).instance()->sheet);
                for (const auto& e : es)
                {
                    sheets_above.insert(e);
                }
            }
            // Then record the direct sheets here
            for (const auto& e : lib.childrenOf(sheet))
            {
                sheets_direct.insert(e);
            }
            for (const auto& e : sheets_above)
            {
                s->sheet(e, lib.nameOf(e), sheets_direct.count(e),
                         tree.canInsertInstance(sheet, e));
            }
        }
    }
    s->pop();
}

////////////////////////////////////////////////////////////////////////////////

std::string Root::toString() const
{
    picojson::value::object obj;
    obj.insert({"type", picojson::value("Straylight")});
    obj.insert({"version", picojson::value((int64_t)1)});

    obj.insert({"root", toJson(SheetIndex(0))});
    return picojson::value(obj).to_str();
}

picojson::value Root::toJson(SheetIndex sheet) const
{
    picojson::value::array items;
    for (auto i : iterItems(sheet))
    {
        picojson::value::object obj;
        if (auto n = getItem(i).instance())
        {
            obj.insert({"type", picojson::value("instance")});
            obj.insert({"sheetIndex", picojson::value((int64_t)n->sheet.i)});
            picojson::value::array inputs;
            for (const auto& p : n->inputs)
            {
                picojson::value::object i;
                i.insert({"cellIndex", picojson::value((int64_t)p.first.i)});
                i.insert({"inputExpr", picojson::value(p.second)});
                inputs.push_back(picojson::value(i));
            }
            obj.insert({"inputs", picojson::value(inputs)});
        }
        else if (auto c = getItem(i).cell())
        {
            obj.insert({"type", picojson::value("cell")});
            obj.insert({"cellIndex", picojson::value((int64_t)i.i)});
            obj.insert({"cellName", picojson::value(tree.nameOf(i))});
            obj.insert({"cellExpr", picojson::value(c->expr)});
        }
        else
        {
            assert(false);
        }
        items.push_back(picojson::value(obj));
    }

    picojson::value::array sheets;
    for (const auto& h : lib.childrenOf(sheet))
    {
        sheets.push_back(toJson(h));
    }

    picojson::value::object out;
    out.insert({"items", picojson::value(items)});
    out.insert({"sheets", picojson::value(sheets)});

    return picojson::value(out);
}

#define REQUIRE(cond, err) if (!(cond)) { return err; }
#define GET(obj, member, type) \
    REQUIRE(obj.count(#member), "'" #member "' must be present"); \
    REQUIRE(obj[#member].is<type>(), "'" #member "' must be " #type); \
    auto member = obj[#member].get<type>();

std::string Root::fromString(const std::string& str)
{
    picojson::value v;
    std::string err = picojson::parse(v, str);
    if (!err.empty())
    {
        return err;
    }

    REQUIRE(v.is<picojson::value::object>(), "Root must be a JSON object");

    auto obj = v.get<picojson::value::object>();
    GET(obj, type, std::string);
    REQUIRE(type == "Straylight", "Invalid type code");

    GET(obj, version, int64_t);
    REQUIRE(version != 1, "Invalid version code");

    REQUIRE(obj.count("root"), "'root' member must be present");
    return fromJson(0, obj["root"]);
}

std::string fromJson(SheetIndex sheet, const picojson::value& value)
{
    REQUIRE(value.is<picojson::value::object>(), "Sheet value must be a JSON object");
    auto obj = value.get<picojson::value::object>();

    REQUIRE(obj.count("item"), "'items' member must be present");

    auto items = obj["items"];
    REQUIRE(items.is<picojson::value::array>(), "'items' member must be JSON array");
    for (const auto& i : items.get<picojson::value::array>())
    {
        REQUIRE(i.is<picojson::value::object>(), "'item' must be JSON object");
        auto item = i.get<picojson::value::object>();
        GET(item, type, std::string);
        if (type == "cell")
        {
            GET(item, cellIndex, int64_t);
            GET(item, cellName, std::string);
            GET(item, cellExpr, std::string);
        }
        else if (t == "instance")
        {
            GET(item, sheetIndex, int64_t);
            GET(item, inputs, picojson::value::array);
            for (auto& n : inputs)
            {
                REQUIRE(n.is<picojson::value::object>(), "'input' must be object");
                auto input = n.get<picojson::value::object>();

                GET(input, cellIndex, int64_t);
                GET(input, inputExpr, std::string);
            }
        }
        else
        {
            return "Unknown item type '" + type + "'";
        }
    }

    REQUIRE(obj.count("sheets"), "'sheets' member must be present");
    auto sheets = obj["sheets"];
    REQUIRE(sheets.is<picojson::value::array>(), "'sheets' item must be JSON array");
    for (const auto& s : sheets.get<picojson::value::array>())
    {
        (void)s;
    }

    return "";
}

////////////////////////////////////////////////////////////////////////////////

void Root::clear()
{
    auto lock = Lock();

    // Erase every item in the root sheet
    const auto items = iterItems({0});
    for (auto i : items)
    {
        if (getItem(i).instance())
        {
            eraseInstance(InstanceIndex(i));
        }
        else if (getItem(i).cell())
        {
            eraseCell(CellIndex(i));
        }
        else
        {
            assert(false);
        }
    }

    const auto sheets = lib.childrenOf(0);
    for (const auto& s : sheets)
    {
        eraseSheet(s);
    }

    assert(dirty.size() == 0);
}

bool Root::checkSheetName(const SheetIndex& parent,
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
    else if (!isupper(name[0]))
    {
        if (err)
        {
            *err = "Name must begin with a uppercase letter";
        }
        return false;
    }
    else if (!lib.canInsert(parent, name))
    {
        if (err)
        {
            *err = "Duplicate name";
        }
        return false;
    }

    return true;
}

SheetIndex Root::insertSheet(const SheetIndex& parent, const std::string& name)
{
    assert(canInsertSheet(parent, name));
    return lib.insert(parent, name);
}

void Root::insertSheet(const SheetIndex& parent, const SheetIndex& sheet,
                       const std::string& name)
{
    assert(canInsertSheet(parent, name));
    return lib.insert(parent, sheet, name);
}

void Root::eraseSheet(const SheetIndex& s)
{
    auto lock = Lock();
    auto instances = tree.instancesOf(s);
    for (const auto& i : instances)
    {
        eraseInstance(i);
    }
    lib.erase(s);
    // Sync is called on lock destruction
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

}   // namespace Graph
