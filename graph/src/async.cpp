#include "graph/async.hpp"
#include "graph/translator.hpp"

namespace Graph {

AsyncRoot::AsyncRoot()
    : Root()
{
    // Announce all of the interpreter's keywords
    for (const auto& k : interpreter.keywords())
    {
        pushChange(Response::ReservedWord(k));
    }

    for (const auto& i : _bad_item_names)
    {
        pushChange(Response::ItemNameRegexBad(i.first, i.second));
        bad_item_names.push_back({std::regex(i.first), i.second});
    }

    for (const auto& i : _bad_sheet_names)
    {
        pushChange(Response::SheetNameRegexBad(i.first, i.second));
        bad_sheet_names.push_back({std::regex(i.first), i.second});
    }
    pushChange(Response::ItemNameRegex(_good_item_name));
    pushChange(Response::SheetNameRegex(_good_sheet_name));
}

void AsyncRoot::insertCell(
        const SheetIndex& sheet, const CellIndex& cell,
        const std::string& name, const std::string& expr)
{
    auto lock = Lock();

    Root::insertCell(sheet, cell, name, expr);
    const auto type = interpreter.cellType(expr);

    pushChange(Response::CellInserted(sheet, cell, name, expr));
    pushChange(Response::CellTypeChanged(sheet, cell, type));

    // Then, mark input / output changes
    if (sheet != Tree::ROOT_SHEET)
    {
        if (type == Cell::INPUT)
        {
            // TODO: this is wrong, as it's the default expr not the input expr
            for (auto i : tree.instancesOf(sheet))
            {
                pushChange(Response::InputCreated(tree.parentOf(i), i, cell,
                            name, interpreter.defaultExpr(expr)));
            }
        }
        else if (type == Cell::OUTPUT)
        {
            for (auto i : tree.instancesOf(sheet))
            {
                pushChange(Response::OutputCreated(tree.parentOf(i), i, cell, name));
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

    pushChange(Response::InstanceInserted(parent, i, target, name, tree.nameOf(target)));
    for (auto item : tree.iterItems(target))
    {
        if (auto c = tree.at(item).cell())
        {
            if (c->type == Cell::INPUT)
            {
                pushChange(Response::InputCreated(
                        parent, i, CellIndex(item), tree.nameOf(item),
                        tree.at(i).instance()->inputs.at(CellIndex(item))));
            }
            else if (c->type == Cell::OUTPUT)
            {
                pushChange(Response::OutputCreated(
                        parent, i, CellIndex(item), tree.nameOf(item)));
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

    pushChange(Response::CellErased(sheet, cell));

    // Mark I/O changes (TODO)
    if (type == Cell::INPUT || type == Cell::OUTPUT)
    {
        for (auto i : tree.instancesOf(sheet))
        {
            pushChange(Response::IOErased(tree.parentOf(i), i, cell));
        }
    }

    // sync occurs on Lock destruction
}

void AsyncRoot::eraseInstance(const InstanceIndex& instance)
{
    auto lock = Lock();

    const auto parent = tree.parentOf(instance);
    const auto sheet = tree.at(instance).instance()->sheet;

    // Notify the changelog about all of the instance IO that disappeared
    for (auto cell : tree.iterItems(sheet))
    {
        if (auto c = tree.at(cell).cell())
        {
            if (c->type == Cell::INPUT || c->type == Cell::OUTPUT)
            {
                    pushChange(Response::IOErased(
                                parent, instance, Graph::CellIndex(cell)));
            }
        }
    }

    Root::eraseInstance(instance);
    pushChange(Response::InstanceErased(parent, instance));
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
    pushChange(Response::ExprChanged(parent, c, expr));

    // If the type changed then pass that info along to the changelog
    if (prev_type != cell->type)
    {
        pushChange(Response::CellTypeChanged(parent, c, cell->type));
    }

    auto d = (cell->type == Cell::INPUT) ? interpreter.defaultExpr(expr) : "";
    if (parent != Tree::ROOT_SHEET)
    {
        for (auto i : tree.instancesOf(parent))
        {
            auto p = tree.parentOf(i);

            if (prev_type != Cell::INPUT && cell->type == Cell::INPUT)
            {
                pushChange(Response::InputCreated(p, i, c, tree.nameOf(c), d));
            }
            else if (prev_type == Cell::INPUT && cell->type != Cell::INPUT)
            {
                pushChange(Response::IOErased(p, i, c));
            }

            if (prev_type != Cell::OUTPUT && cell->type == Cell::OUTPUT)
            {
                pushChange(Response::OutputCreated(p, i, c, tree.nameOf(c)));
            }
            else if (prev_type == Cell::OUTPUT && cell->type != Cell::OUTPUT)
            {
                pushChange(Response::IOErased(p, i, c));
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
        pushChange(Response::InputExprChanged(parent, instance, cell, expr));
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
            pushChange(Response::CellRenamed(
                        parent, CellIndex(item), name));

            if (c->type == Cell::INPUT || c->type == Cell::OUTPUT)
            {
                for (auto i : tree.instancesOf(parent))
                {
                    pushChange(Response::IORenamed(
                        tree.parentOf(i), i, CellIndex(item), name));
                }
            }
        }
        else
        {
            pushChange(Response::InstanceRenamed(
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

    pushChange(Response::SheetCreated(parent, sheet, name));

    for (auto b : tree.sheetsBelow(parent))
    {
        if (b != parent)
        {
            pushChange(Response::SheetAvailable(b, sheet, name, b != sheet));
        }
    }

    for (auto a : tree.sheetsAbove(parent))
    {
        if (a != sheet)
        {
            pushChange(Response::SheetAvailable(sheet, a, tree.nameOf(a), true));
        }
    }
}

void AsyncRoot::renameSheet(const SheetIndex& sheet, const std::string& name)
{
    auto lock = Lock();
    Root::renameSheet(sheet, name);

    for (auto s : tree.sheetsBelow(tree.parentOf(sheet)))
    {
        pushChange(Response::SheetRenamed(s, sheet, name));
    }

    // Inform all instances of this sheet that it has been renamed
    for (const auto& i : tree.instancesOf(sheet))
    {
        pushChange(Response::InstanceSheetRenamed(tree.parentOf(i), i, name));
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
            pushChange(Response::SheetUnavailable(b, s));
        }
    }

    for (auto a : tree.sheetsAbove(p))
    {
        if (a != s)
        {
            pushChange(Response::SheetUnavailable(s, a));
        }
    }

    pushChange(Response::SheetErased(p, s));
}

void AsyncRoot::gotResult(const CellKey& k, const Value& result)
{
    Root::gotResult(k, result);

    // Avoid leaking translated values by only constructing them
    // if we're connected (instead of using pushChange)
    if (dirty.size() == 1)
    {
            auto escaped = translator
                ? (*translator)(Interpreter::untag(result.value))
                : nullptr;
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
}

void AsyncRoot::serialize()
{
    pushChange(Response::Serialized(tree.toString()));
}


std::string AsyncRoot::loadString(const std::string& json)
{
    auto lock = Lock();

    auto err = Root::loadString(json);
    pushChange(Response::Serialized(err));

    ////////////////////////////////////////////////////////////////////////////
    // First, announce all of the sheets, building the skeleton of the graph
    std::list<SheetIndex> sheets;
    std::list<CellIndex> cells;
    {
        std::list<SheetIndex> todo = {Tree::ROOT_SHEET};
        while (todo.size())
        {
            auto sheet = todo.front();
            todo.pop_front();

            for (auto i : tree.childrenOf(sheet))
            {
                if (tree.at(i).sheet())
                {
                    todo.push_back(SheetIndex(i));
                }
            }
            if (sheet != Tree::ROOT_SHEET)
            {
                pushChange(Response::SheetCreated(
                            tree.parentOf(sheet), sheet, tree.nameOf(sheet)));
            }
            sheets.push_back(sheet);
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    for (auto sheet : sheets)
    {
        //  Cross-link sheets network based on what can be inserted where,
        if (sheet != Tree::ROOT_SHEET)
        {
            auto parent = tree.parentOf(sheet);
            auto name = tree.nameOf(sheet);
            for (auto b : tree.sheetsBelow(sheet))
            {
                if (b != parent)
                {
                    pushChange(Response::SheetAvailable(
                                b, sheet, name, b != sheet));
                }
            }

            for (auto a : tree.sheetsAbove(parent))
            {
                if (a != sheet)
                {
                    pushChange(Response::SheetAvailable(
                                sheet, a, tree.nameOf(a), true));
                }
            }
        }

        // Announce instance construction, which needs to be done before
        // cells so that I/O cells have a place to put themselves
        for (auto i : tree.iterItems(sheet))
        {
            auto name = tree.nameOf(i);
            if (tree.at(i).cell())
            {
                cells.push_back(CellIndex(i));
            }
            else if (auto n = tree.at(i).instance())
            {
                pushChange(Response::InstanceInserted(
                            sheet, InstanceIndex(i), n->sheet,
                            name, tree.nameOf(n->sheet)));
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    //  Announce every cell and their input/output properties
    for (auto cell : cells)
    {
        auto name = tree.nameOf(cell);
        auto c = tree.at(cell).cell();
        auto parent = tree.parentOf(cell);

        pushChange(Response::CellInserted(parent, cell, name, c->expr));
        pushChange(Response::CellTypeChanged(parent, cell, c->type));

        if (parent != Tree::ROOT_SHEET)
        {
            for (auto i : tree.instancesOf(parent))
            {
                auto p = tree.parentOf(i);
                if (c->type == Cell::INPUT)
                {
                    pushChange(Response::InputCreated(p, i, cell, name,
                                tree.at(i).instance()->inputs.at(cell)));
                }
                else if (c->type == Cell::OUTPUT)
                {
                    pushChange(Response::OutputCreated(p, i, cell, name));
                }
            }
        }
    }

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

void AsyncRoot::pushChange(const Response& response)
{
    // Only push changes through to the UI when we're not evaluating
    // a nested sheet (with callSheet)
    if (dirty.size() == 1)
    {
        changes.push(response);
    }
}

}   // namespace Graph
