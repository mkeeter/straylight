#include "graph/symbol.hpp"
#include "graph/sheet.hpp"

namespace Graph
{

SymbolTable::SymbolTable(const Root& r, Dependencies& d, const CellKey& t)
    : root(r), deps(d), target(t),
      sheet(root.sheets.at(root.cells.at(target.id)->parent).get())
{
    // Since a symbol table is only constructed before an evaluation,
    // clear target's dependencies here.  They'll be re-populated by
    // SymbolTable::get, which is called through the unbound variable
    // hook in the interpreter.
    deps.erase(target);
}

std::pair<Interpreter::Value, SymbolTable::Result>
SymbolTable::get(const std::string& symbol)
{
    auto c = sheet->cells.left.find(symbol);
    if (c != sheet->cells.left.end())
    {
        const CellId cell_id = c->second;

        if (deps.insert(target, {cell_id, target.env}))
        {
            return {nullptr, RECURSIVE};
        }

        const auto& cell = *root.cells.at(cell_id);
        auto v = cell.values.find(target.env);
        if (v == cell.values.end())
        {
            auto itr = cell.values.lower_bound(target.env);
            if (itr != cell.values.end() &&
                CellKey(target.id, itr->first).specializes(target))
            {
                auto b = itr;

                while (b != cell.values.end() &&
                       CellKey(target.id, b->first).specializes(target))
                {
                    ++b;
                }
                todo = {itr, b};
                return {nullptr, MULTIPLE_VALUES};
            }
            else if (itr != cell.values.begin() &&
                     target.specializes(CellKey(target.id, (--itr)->first)))
            {
                return {itr->second.value, OKAY};
            }
            return {nullptr, MISSING_ENV};
        }

        // Return cell value
        return {v->second.value, OKAY};
    }

    auto i = sheet->instances.left.find(symbol);
    if (i != sheet->instances.left.end())
    {
        // TODO: Return instance thunk
    }

    // TODO: call sheet operation here

    return {nullptr, NO_SUCH_NAME };
}

}   // namespace Graph
