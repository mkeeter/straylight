#include "graph/symbol.hpp"
#include "graph/sheet.hpp"

namespace Graph
{

SymbolTable::SymbolTable(Graph& g, const CellKey& t)
    : graph(g), target(t), sheet(graph.sheet(graph.cell(target.id)->parent))
{
    // Nothing to do here
}

Interpreter::Value SymbolTable::get(const std::string& symbol)
{
    if (graph.isItemName(symbol))
    {
        auto c = sheet->cells.find(symbol);
        if (c != sheet->cells.end())
        {
            // Return cell value
        }

        auto i = sheet->instances.find(symbol);
        if (i != sheet->instances.end())
        {
            // Return instance thunk
        }
    }
    else if (graph.isSheetName(symbol))
    {
        // TODO
    }
}

}   // namespace Graph
