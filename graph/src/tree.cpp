#include "graph/tree.hpp"

#include "graph/types/instance.hpp"
#include "graph/types/cell.hpp"

namespace Graph {

const SheetIndex Tree::ROOT_SHEET = 0;
const InstanceIndex Tree::ROOT_INSTANCE = 1;

Tree::Tree()
{
    reset();
}

Tree::~Tree()
{
    for (auto& i : storage)
    {
        i.second.dealloc();
    }
}

void Tree::reset()
{
    KeyNameStore::reset();
    order.clear();
    storage.insert({ROOT_INSTANCE, Item(ROOT_SHEET)});
    storage.insert({ROOT_SHEET, Item()});
}

/*
 *  Insert a new cell with the given expression
 */
CellIndex Tree::insertCell(const SheetIndex& parent, const std::string& name,
                           const std::string& expr)
{
    auto c = insert(parent, name, Item(expr));
    order[parent].push_back(c);
    return CellIndex(c);
}

void Tree::insertCell(const SheetIndex& parent, const CellIndex& cell,
                      const std::string& name, const std::string& expr)
{
    insert(parent, cell, name, Item(expr));
    order[parent].push_back(cell);
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
    return InstanceIndex(i);
}

void Tree::insertInstance(const SheetIndex& parent, const InstanceIndex& i,
                          const std::string& name, const SheetIndex& target)
{
    insert(parent, i, name, Item(target));
    order[parent].push_back(i);
}

bool Tree::canInsertInstance(const SheetIndex& parent,
                             const SheetIndex& target) const
{
    for (const auto& e : envsOf(parent))
    {
        for (const auto& s : e)
        {
            if (target == at(s).instance()->sheet)
            {
                return false;
            }
        }
    }
    return true;
}

const std::list<ItemIndex>& Tree::iterItems(const SheetIndex& parent) const
{
    const static std::list<ItemIndex> empty;
    return order.count(parent) ? order.at(parent) : empty;
}

std::list<Env> Tree::envsOf(const SheetIndex& s) const
{
    std::list<std::pair<Env, SheetIndex>> todo =
            {{{ROOT_INSTANCE}, ROOT_SHEET}};
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
                    env_.push_back(InstanceIndex(i));
                    todo.push_back({env_, instance->sheet});
                }
            }
        }

        todo.pop_front();
    }

    return found;
}

std::list<InstanceIndex> Tree::instancesOf(const SheetIndex& s) const
{
    std::list<InstanceIndex> out;
    for (const auto& i : storage)
    {
        if (i.second.instance() && i.second.instance()->sheet == s)
        {
            out.push_back(InstanceIndex(i.first));
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

std::list<CellKey> Tree::iterCellsRecursive(const SheetIndex& s) const
{
    std::list<CellKey> out;
    for (auto i : iterItems(s))
    {
        if (at(i).cell())
        {
            out.push_back({{}, CellIndex(i)});
        }
        else if (auto n = at(i).instance())
        {
            for (auto k : iterCellsRecursive(n->sheet))
            {
                k.first.push_front(InstanceIndex(i));
                out.push_back(k);
            }
        }
    }
    return out;
}

std::list<ItemIndex> Tree::iterAll() const
{
    std::list<ItemIndex> out;
    for (const auto& i : storage)
    {
        out.push_back(i.first);
    }
    return out;
}

std::list<std::pair<Env, ItemIndex>> Tree::iterItemsRecursive(const SheetIndex& s) const
{
    std::list<std::pair<Env, ItemIndex>> out;
    for (auto i : iterItems(s))
    {
        out.push_back({{}, i});
        if (auto n = at(i).instance())
        {
            for (auto k : iterItemsRecursive(n->sheet))
            {
                k.first.push_front(InstanceIndex(i));
                out.push_back(k);
            }
        }
    }
    return out;
}

////////////////////////////////////////////////////////////////////////////////

std::string Tree::toString() const
{
    picojson::value::object obj;
    obj.insert({"type", picojson::value("Straylight")});
    obj.insert({"version", picojson::value((int64_t)1)});

    obj.insert({"root", toJson(Tree::ROOT_SHEET)});
    return picojson::value(obj).serialize();
}

picojson::value Tree::toJson(SheetIndex sheet) const
{
    picojson::value::array items;
    picojson::value::array sheets;

    for (auto i : iterItems(sheet))
    {
        picojson::value::object obj;
        obj.insert({"itemIndex", picojson::value((int64_t)i.i)});
        obj.insert({"itemName", picojson::value(nameOf(i))});

        if (auto n = at(i).instance())
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
        else if (auto c = at(i).cell())
        {
            obj.insert({"type", picojson::value("cell")});
            obj.insert({"cellExpr", picojson::value(c->expr)});
        }
        else
        {
            assert(false);
        }
        items.push_back(picojson::value(obj));
    }

    for (auto i : childrenOf(sheet))
    {
        if (at(i).sheet())
        {
            sheets.push_back(toJson(SheetIndex(i)));
        }
    }

    picojson::value::object out;
    out.insert({"items", picojson::value(items)});
    out.insert({"sheets", picojson::value(sheets)});

    // For nested sheets, save their name and index
    if (sheet.i)
    {
        out.insert({"sheetIndex", picojson::value((int64_t)sheet.i)});
        out.insert({"sheetName", picojson::value(nameOf(sheet))});
    }

    return picojson::value(out);
}

#define REQUIRE(cond, err) if (!(cond)) { return err; }
#define GET(obj, member, type) \
    REQUIRE(obj.count(#member), "'" #member "' must be present"); \
    REQUIRE(obj[#member].is<type>(), "'" #member "' must be " #type); \
    auto member = obj[#member].get<type>();

std::string Tree::fromString(const std::string& str)
{
    assert(storage.size() == 2);
    assert(storage.rbegin()->first.i == 1);

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
    REQUIRE(version == 1, "Invalid version code");

    REQUIRE(obj.count("root"), "'root' member must be present");
    return fromJson(Tree::ROOT_SHEET, obj["root"]);
}

std::string Tree::fromJson(SheetIndex sheet, const picojson::value& value)
{
    REQUIRE(value.is<picojson::value::object>(), "Sheet value must be a JSON object");
    auto obj = value.get<picojson::value::object>();

    auto items = obj["items"];
    REQUIRE(items.is<picojson::value::array>(), "'items' member must be JSON array");
    for (const auto& i : items.get<picojson::value::array>())
    {
        REQUIRE(i.is<picojson::value::object>(), "'item' must be JSON object");
        auto item = i.get<picojson::value::object>();

        // Common to all items
        GET(item, type, std::string);
        GET(item, itemIndex, int64_t);
        GET(item, itemName, std::string);

        if (type == "cell")
        {
            GET(item, cellExpr, std::string);
            insertCell(sheet, itemIndex, itemName, cellExpr);
        }
        else if (type == "instance")
        {
            GET(item, sheetIndex, int64_t);
            GET(item, inputs, picojson::value::array);
            insertInstance(sheet, itemIndex, itemName, sheetIndex);
            for (auto& n : inputs)
            {
                REQUIRE(n.is<picojson::value::object>(), "'input' must be object");
                auto input = n.get<picojson::value::object>();

                GET(input, cellIndex, int64_t);
                GET(input, inputExpr, std::string);
                at(itemIndex).instance()->inputs[cellIndex] = inputExpr;
            }
        }
        else
        {
            return "Unknown item type '" + type + "'";
        }
    }

    GET(obj, sheets, picojson::value::array);
    for (const auto& s : sheets)
    {
        REQUIRE(s.is<picojson::value::object>(), "'sheet' must be object");
        auto obj = s.get<picojson::value::object>();

        GET(obj, sheetIndex, int64_t);
        GET(obj, sheetName, std::string);
        insert(sheet, sheetIndex, sheetName, Item());
        fromJson(sheetIndex, s);
    }

    return "";
}

////////////////////////////////////////////////////////////////////////////////

std::list<SheetIndex> Tree::sheetsAbove(const SheetIndex& sheet) const
{
    std::list<SheetIndex> out;

    auto p = sheet;
    while (true)
    {
        for (const auto& e : childrenOf(p))
        {
            if (at(e).sheet())
            {
                out.push_back(SheetIndex(e));
            }
        }

        if (p == ROOT_SHEET)
        {
            break;
        }
        else
        {
            p = parentOf(p);
        }
    }

    return out;
}

std::list<SheetIndex> Tree::sheetsBelow(const SheetIndex& parent) const
{
    std::list<SheetIndex> todo = {parent};
    std::list<SheetIndex> done;

    while(todo.size())
    {
        auto s = todo.front();
        for (auto i : childrenOf(s))
        {
            if (at(i).sheet())
            {
                todo.push_back(SheetIndex(i));
            }
        }
        done.push_back(s);
        todo.pop_front();
    }
    return done;
}

bool Tree::checkEnv(const Env& env) const
{
    return std::all_of(env.begin(), env.end(),
            [=](const InstanceIndex& i)
            { return isValid(i) && at(i).instance(); });
}


}   // namespace Graph
