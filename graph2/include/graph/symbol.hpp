#pragma once

#include "graph/graph.hpp"
#include "graph/interpreter.hpp"
#include "graph/key.hpp"

namespace Graph
{

class SymbolTable
{
public:
    /*
     *  Construct a symbol table, clearing the target's dependencies
     */
    SymbolTable(const Root& r, Dependencies& deps, const CellKey& t);

    /*
     *  Looks up the value in our given environment
     *  Records the lookup in dependencies map.
     */
    enum Result { OKAY, RECURSIVE, NO_SUCH_NAME, MISSING_ENV, MULTIPLE_VALUES };
    std::pair<Interpreter::Value, Result> get(const std::string& symbol);

    /*
     *  Looks up the previous value associated with our target
     */
    Value& prev() const;

    /*
     *  Checks whether there are specialized keys in need of evaluation
     */
    const std::list<CellKey>& todo() const { return _todo; }

protected:
    const Root& root;
    Dependencies& deps;

    const CellKey target;
    Sheet* sheet;

    /*  If a lookup produced more than one valid result, then they are stored
     *  here in _todo.  It is the caller's responsability to check this and
     *  re-schedule them for evaluation  */
    std::list<CellKey> _todo;
};

}   // namespace Graph
