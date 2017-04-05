#pragma once

#include "graph/graph.hpp"
#include "graph/interpreter.hpp"
#include "graph/key.hpp"

namespace Graph
{

class SymbolTable
{
public:
    SymbolTable(Graph& g, const CellKey& t);

    /*
     *  Looks up the value in our given environment
     *  The result could be an error or a valid result
     */
    Interpreter::Value get(const std::string& symbol);

    /*
     *  Looks up the previous value associated with our target
     */
    Value& prev() const;

    /*
     *  Checks whether there are specialized keys in need of evaluation
     */
    const std::list<CellKey>& todo() const { return _todo; }

protected:
    Graph& graph;
    const CellKey target;
    Sheet* sheet;

    /*  If a lookup produced more than one valid result, then they are stored
     *  here in _todo.  It is the caller's responsability to check this and
     *  re-schedule them for evaluation  */
    std::list<CellKey> _todo;
};

}   // namespace Graph
