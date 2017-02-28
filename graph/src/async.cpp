#include "graph/async.hpp"
#include "graph/translator.hpp"

namespace Graph {

AsyncRoot::AsyncRoot()
    : Root()
{
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

    changes.push(Response::CellInserted(sheet, cell, name, expr));
    changes.push(Response::CellTypeChanged(sheet, cell, type));

    // Then, mark input / output changes
    if (sheet != Tree::ROOT_SHEET)
    {
        if (type == Cell::INPUT)
        {
            // TODO: this is wrong, as it's the default expr not the input expr
            for (auto i : tree.instancesOf(sheet))
            {
                changes.push(Response::InputCreated(tree.parentOf(i), i, cell,
                            name, interpreter.defaultExpr(expr)));
            }
        }
        else if (type == Cell::OUTPUT)
        {
            for (auto i : tree.instancesOf(sheet))
            {
                changes.push(Response::OutputCreated(tree.parentOf(i), i, cell, name));
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

    changes.push(Response::InstanceInserted(parent, i, target, name, tree.nameOf(target)));

    // TODO: something with IO here?

    // sync occurs on Lock destruction
}

void AsyncRoot::eraseCell(const CellIndex& cell)
{
    auto lock = Lock();

    const auto type = tree.at(cell).cell()->type;
    const auto sheet = tree.parentOf(cell);
    Root::eraseCell(cell);

    changes.push(Response::CellErased(sheet, cell));

    // Mark I/O changes (TODO)
    if (type == Cell::INPUT || type == Cell::OUTPUT)
    {
        for (auto i : tree.instancesOf(sheet))
        {
            changes.push(Response::IOErased(tree.parentOf(i), i, cell));
        }
    }

    // sync occurs on Lock destruction
}

void AsyncRoot::eraseInstance(const InstanceIndex& instance)
{
    auto lock = Lock();

    const auto parent = tree.parentOf(instance);
    Root::eraseInstance(instance);
    changes.push(Response::InstanceErased(parent, instance));
}

bool AsyncRoot::setExpr(const CellIndex& c, const std::string& expr)
{
    auto lock = Lock();

    auto cell = tree.at(c).cell();
    auto parent = tree.parentOf(c);
    const auto prev_type = cell->type;

    if (!Root::setExpr(c, expr))
    {
        return false;
    }

    // Mark that the expression itself has changed
    changes.push(Response::ExprChanged(parent, c, expr));

    // If the type changed then pass that info along to the changelog
    if (prev_type != cell->type)
    {
        changes.push(Response::CellTypeChanged(parent, c, cell->type));
    }

    auto d = (cell->type == Cell::INPUT) ? interpreter.defaultExpr(expr) : "";
    if (parent != Tree::ROOT_SHEET)
    {
        for (auto i : tree.instancesOf(parent))
        {
            auto p = tree.parentOf(i);

            if (prev_type != Cell::INPUT && cell->type == Cell::INPUT)
            {
                changes.push(Response::InputCreated(p, i, c, tree.nameOf(c), d));
            }
            else if (prev_type == Cell::INPUT && cell->type != Cell::INPUT)
            {
                changes.push(Response::IOErased(p, i, c));
            }

            if (prev_type != Cell::OUTPUT && cell->type == Cell::OUTPUT)
            {
                changes.push(Response::OutputCreated(p, i, c, tree.nameOf(c)));
            }
            else if (prev_type == Cell::OUTPUT && cell->type != Cell::OUTPUT)
            {
                changes.push(Response::IOErased(p, i, c));
            }
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
        for (auto i : tree.instancesOf(parent))
        {
            changes.push(Response::InputExprChanged(tree.parentOf(i), i, cell,
                        expr));
        }
        return true;
    }
}

bool AsyncRoot::renameItem(const ItemIndex& item, const std::string& name)
{
    auto lock = Lock();
    if (!Root::renameItem(item, name))
    {
        return false;
    }
    else
    {
        auto parent = tree.parentOf(item);

        if (auto c = tree.at(item).cell())
        {
            changes.push(Response::CellRenamed(
                        parent, CellIndex(item), name));

            if (c->type == Cell::INPUT || c->type == Cell::OUTPUT)
            {
                for (auto i : tree.instancesOf(parent))
                {
                    changes.push(Response::IORenamed(
                        tree.parentOf(i), i, CellIndex(item), name));
                }
            }
        }
        else
        {
            changes.push(Response::InstanceRenamed(
                    parent, InstanceIndex(item), name));
        }
        return true;
    }
}

void AsyncRoot::insertSheet(const SheetIndex& parent, const SheetIndex& sheet,
                            const std::string& name)
{
    auto lock = Lock();
    Root::insertSheet(parent, sheet, name);

    changes.push(Response::SheetCreated(parent, sheet, name));

    for (auto b : tree.sheetsBelow(parent))
    {
        if (b != parent)
        {
            changes.push(Response::SheetAvailable(b, sheet, name, b != sheet));
        }
    }

    for (auto a : tree.sheetsAbove(parent))
    {
        if (a != sheet)
        {
            changes.push(Response::SheetAvailable(sheet, a, tree.nameOf(a), true));
        }
    }
}

void AsyncRoot::renameSheet(const SheetIndex& sheet, const std::string& name)
{
    auto lock = Lock();
    Root::renameSheet(sheet, name);

    for (auto s : tree.sheetsBelow(tree.parentOf(sheet)))
    {
        changes.push(Response::SheetRenamed(s, sheet, name));
    }

    // Inform all instances of this sheet that it has been renamed
    for (const auto& i : tree.instancesOf(sheet))
    {
        changes.push(Response::InstanceSheetRenamed(tree.parentOf(i), i, name));
    }
}

void AsyncRoot::eraseSheet(const SheetIndex& s)
{
    auto lock = Lock();

    auto p = tree.parentOf(s);
    Root::eraseSheet(s);

    for (auto b : tree.sheetsBelow(p))
    {
        if (b != p)
        {
            changes.push(Response::SheetUnavailable(b, s));
        }
    }

    for (auto a : tree.sheetsAbove(p))
    {
        if (a != s)
        {
            changes.push(Response::SheetUnavailable(s, a));
        }
    }

    changes.push(Response::SheetErased(p, s));
}

void AsyncRoot::gotResult(const CellKey& k, const Value& result)
{
    Root::gotResult(k, result);

    auto escaped = translator ? (*translator)(result.value) : nullptr;
    changes.push(Response::ValueChanged(
                tree.parentOf(k.second), k, result.str, result.valid,
                escaped));

    // Announce new values if this is an IO cell
    auto c = tree.at(k.second).cell();
    if (c->type == Cell::INPUT || c->type == Cell::OUTPUT)
    {
        for (auto i : tree.instancesOf(tree.parentOf(k.second)))
        {
            changes.push(Response::IOValueChanged(
                tree.parentOf(i), i, CellIndex(k.second), k.first,
                result.str, result.valid, escaped));
        }
    }
}

void AsyncRoot::serialize()
{
    changes.push(Response::Serialized(tree.toString()));
}


std::string AsyncRoot::loadString(const std::string& json)
{
    auto lock = Lock();

    auto err = Root::loadString(json);
    changes.push(Response::Serialized(err));

    // XXX package up the entire graph and write it out here
    // This should be fun...

    if (err.size())
    {
        clear();
    }
    return err;
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
