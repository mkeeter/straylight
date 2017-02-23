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

    for (const auto& e : tree.envsOf(parent))
    {
        changes.push(Response::InstanceInserted(
                    e, i, target, name, tree.nameOf(target)));

        for (const auto& c : tree.iterCellsRecursive(target))
        {
            auto env = e; // copy
            env.push_back(i);
            env.insert(env.end(), c.first.begin(), c.first.end());

            const auto name = tree.nameOf(c.second);
            const auto expr = tree.at(c.second).cell()->expr;
            changes.push(Response::CellInserted({env, c.second}, name, expr));
        }
    }

    // Send information about every instance's IO
    for (const auto& t : tree.iterItems(target))
    {
        if (auto c = tree.at(t).cell())
        {
            if (c->type == Cell::INPUT)
            {
                const auto expr = interpreter.defaultExpr(c->expr);
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
        changes.push(Response::ItemErased(e, cell));

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
        changes.push(Response::ItemErased(env, instance));
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
        for (const auto& env : tree.envsOf(tree.parentOf(i)))
        {
            changes.push(Response::ItemRenamed(env, i, name));
        }
        return true;
    }
}

void AsyncRoot::insertSheet(const SheetIndex& parent, const SheetIndex& sheet,
                            const std::string& name)
{
    auto lock = Lock();
    Root::insertSheet(parent, sheet, name);

    for (const auto& e : tree.envsOf(parent))
    {
        changes.push(Response::SheetCreated(e, sheet, name));
        // TODO: also insert sheet into children libraries
    }
}

void AsyncRoot::renameSheet(const SheetIndex& sheet, const std::string& name)
{
    auto lock = Lock();
    Root::renameSheet(sheet, name);

    for (const auto& e : tree.envsOf(tree.parentOf(sheet)))
    {
        changes.push(Response::SheetRenamed(e, sheet, name));
        // TODO: also rename in children libraries
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

    const auto envs = tree.envsOf(tree.parentOf(s));
    Root::eraseSheet(s);

    for (const auto& e : envs)
    {
        changes.push(Response::SheetErased(e, s));
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
