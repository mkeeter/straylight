#include "graph/async.hpp"

namespace Graph {

AsyncRoot::AsyncRoot()
    : Root()
{
    // Announce the creation of the root instance
    changes.push(Response::InstanceInserted(
                {}, Tree::ROOT_INSTANCE, Tree::ROOT_SHEET,
                "Root", ""));

    // Announce all of the interpreter's keywords
    for (const auto& k : interpreter.keywords())
    {
        changes.push(Response::ReservedWord(k));
    }

    for (const auto& i : _bad_item_names)
    {
        changes.push(Response::ItemNameRegexBad(i.first, i.second));
        bad_item_names.push_back({std::regex(i.first), i.second});
    }

    for (const auto& i : _bad_sheet_names)
    {
        changes.push(Response::SheetNameRegexBad(i.first, i.second));
        bad_sheet_names.push_back({std::regex(i.first), i.second});
    }
    changes.push(Response::ItemNameRegex(_good_item_name));
    changes.push(Response::SheetNameRegex(_good_sheet_name));
}

void AsyncRoot::insertCell(
        const SheetIndex& sheet, const CellIndex& cell,
        const std::string& name, const std::string& expr)
{
    auto lock = Lock();

    Root::insertCell(sheet, cell, name, expr);
    const auto type = interpreter.cellType(expr);

    for (const auto& e : tree.envsOf(sheet))
    {
        changes.push(Response::CellInserted({e, cell}, name, expr));
        changes.push(Response::CellTypeChanged({e, cell}, type));
    }

    if (sheet != Tree::ROOT_SHEET)
    {
        // Then, mark input / output changes
        for (const auto& e : tree.envsOf(sheet))
        {
            if (type == Cell::INPUT)
            {
                // TODO: this is wrong, as it's the default expr not the input expr
                // Does this matter?
                changes.push(Response::InputCreated({e, cell}, name,
                             interpreter.defaultExpr(expr)));
            }
            else if (type == Cell::OUTPUT)
            {
                changes.push(Response::OutputCreated({e, cell}, name));
            }
        }
    }

    // Evaluation happens on lock destruction
}

void AsyncRoot::insertInstance(
        const SheetIndex& parent, const InstanceIndex& i,
        const std::string& name, const SheetIndex& target)
{
    auto lock = Lock();

    Root::insertInstance(parent, i, name, target);

    // Find all items relative to the new instance
    auto items = tree.iterItemsRecursive(target);
    for (auto& item : items)
    {
        item.first.push_front(i);
    }
    items.push_front({{}, i});

    // Recursively construct all the children in all the environments
    for (const auto& e : tree.envsOf(parent))
    {
        for (const auto& item : items)
        {
            auto env = e; // copy
            env.insert(env.end(), item.first.begin(), item.first.end());

            const auto name = tree.nameOf(item.second);

            if (auto c = tree.at(item.second).cell())
            {
                const auto expr = c->expr;
                changes.push(Response::CellInserted(
                            {env, CellIndex(item.second)}, name, expr));

                if (c->type == Cell::INPUT)
                {
                    const auto expr = interpreter.defaultExpr(c->expr);
                    changes.push(Response::InputCreated(
                                {env, CellIndex(item.second)}, name, expr));
                }
                else if (c->type == Cell::OUTPUT)
                {
                    changes.push(Response::OutputCreated(
                                {env, CellIndex(item.second)}, name));
                }
            }
            else if (auto instance = tree.at(item.second).instance())
            {
                changes.push(Response::InstanceInserted(
                            env, InstanceIndex(item.second), instance->sheet,
                            name, tree.nameOf(instance->sheet)));

                for (auto s : tree.sheetsAbove(instance->sheet))
                {
                    auto env_ = env;
                    env_.push_back(InstanceIndex(item.second));
                    changes.push(Response::SheetCreated(env_, s, tree.nameOf(s),
                        s == parent, true)); // TODO: insertable is wrong here
                }
            }
        }
    }

    // sync occurs on Lock destruction
}

void AsyncRoot::eraseCell(const CellIndex& cell)
{
    auto lock = Lock();

    const auto type = tree.at(cell).cell()->type;
    const auto sheet = tree.parentOf(cell);
    Root::eraseCell(cell);

    for (const auto& e : tree.envsOf(sheet))
    {
        changes.push(Response::CellErased(e, cell));

        // Mark I/O changes
        if (type == Cell::INPUT || type == Cell::OUTPUT)
        {
            changes.push(Response::IOErased({e, cell}));
        }
    }
    // sync occurs on Lock destruction
}

void AsyncRoot::eraseInstance(const InstanceIndex& instance)
{
    auto lock = Lock();

    const std::string name = tree.nameOf(instance);
    const auto parent = tree.parentOf(instance);

    Root::eraseInstance(instance);

    for (auto env : tree.envsOf(parent))
    {
        changes.push(Response::InstanceErased(env, instance));
    }
}

bool AsyncRoot::setExpr(const CellIndex& c, const std::string& expr)
{
    auto lock = Lock();

    auto cell = tree.at(c).cell();
    const auto prev_type = cell->type;

    if (!Root::setExpr(c, expr))
    {
        return false;
    }

    auto d = (cell->type == Cell::INPUT) ? interpreter.defaultExpr(expr) : "";

    for (const auto& e : tree.envsOf(tree.parentOf(c)))
    {
        // Mark that the expression itself has changed
        changes.push(Response::ExprChanged({e, c}, expr));

        // If the type changed then pass that info along to the changelog
        if (prev_type != cell->type)
        {
            changes.push(Response::CellTypeChanged({e, c}, cell->type));
        }

        // Handle IO state changes
        if (prev_type != Cell::INPUT && cell->type == Cell::INPUT)
        {
            changes.push(Response::InputCreated({e, c}, tree.nameOf(c), d));
        }
        else if (prev_type == Cell::INPUT && cell->type != Cell::INPUT)
        {
            changes.push(Response::IOErased({e, c}));
        }
        if (prev_type != Cell::OUTPUT && cell->type == Cell::OUTPUT)
        {
            changes.push(Response::OutputCreated({e, c}, tree.nameOf(c)));
        }
        else if (prev_type == Cell::OUTPUT && cell->type != Cell::OUTPUT)
        {
            changes.push(Response::IOErased({e, c}));
        }
    }

    return true;
}

bool AsyncRoot::setInput(const InstanceIndex& instance, const CellIndex& cell,
                         const std::string& expr)
{
    auto lock = Lock();
    const auto parent = tree.parentOf(instance);

    if (!Root::setInput(instance, cell, expr))
    {
        return false;
    }
    else
    {
        for (auto env : tree.envsOf(parent))
        {
            changes.push(Response::InputChanged({env, cell}, expr));
        }
        return true;
    }
}

bool AsyncRoot::renameItem(const ItemIndex& i, const std::string& name)
{
    auto lock = Lock();
    if (!Root::renameItem(i, name))
    {
        return false;
    }
    else
    {
        bool is_cell = tree.at(i).cell();
        for (const auto& env : tree.envsOf(tree.parentOf(i)))
        {
            if (is_cell)
            {
                changes.push(
                        Response::CellRenamed(env, CellIndex(i), name));
            }
            else
            {
                changes.push(
                        Response::InstanceRenamed(env, InstanceIndex(i), name));
            }
        }
        return true;
    }
}

void AsyncRoot::insertSheet(const SheetIndex& parent, const SheetIndex& sheet,
                            const std::string& name)
{
    auto lock = Lock();
    Root::insertSheet(parent, sheet, name);

    for (auto s : tree.sheetsBelow(parent))
    {
        for (const auto& e : tree.envsOf(s))
        {
            changes.push(Response::SheetCreated(e, sheet, name,
                        s == parent, s != sheet));
        }
    }
}

void AsyncRoot::renameSheet(const SheetIndex& sheet, const std::string& name)
{
    auto lock = Lock();
    Root::renameSheet(sheet, name);

    for (auto s : tree.sheetsBelow(tree.parentOf(sheet)))
    {
        for (const auto& e : tree.envsOf(s))
        {
            changes.push(Response::SheetRenamed(e, sheet, name));
        }
    }

    // Inform all instances of this sheet that it has been renamed
    for (const auto& i : tree.instancesOf(sheet))
    {
        for (const auto& e : tree.envsOf(tree.parentOf(i)))
        {
            changes.push(Response::InstanceSheetRenamed(e, i, name));
        }
    }
}

void AsyncRoot::eraseSheet(const SheetIndex& s)
{
    auto lock = Lock();

    auto p = tree.parentOf(s);
    Root::eraseSheet(s);

    for (auto s : tree.sheetsBelow(p))
    {
        for (const auto& e : tree.envsOf(s))
        {
            changes.push(Response::SheetErased(e, s));
        }
    }
}

void AsyncRoot::gotResult(const CellKey& k, const Value& result)
{
    Root::gotResult(k, result);
    changes.push(Response::ValueChanged(k, result.str, result.valid));
}

////////////////////////////////////////////////////////////////////////////////

shared_queue<Response>& AsyncRoot::run(shared_queue<Command>& input)
{
    future = std::async(std::launch::async, [&]{ _run(input); });
    return changes;
}

void AsyncRoot::wait()
{
    future.wait();
}

void AsyncRoot::_run(shared_queue<Command>& input)
{
    while (true)
    {
        input.wait();
        if (input.done())
        {
            changes.halt();
            break;
        }
        else
        {
            auto cmd = input.pop();
            cmd(*this);
        }
    }
}

}   // namespace Graph
