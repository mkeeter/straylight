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
     *  Return an iterator range for specialized evaluation
     */
    typedef std::map<Cell::Env, Value>::const_iterator itr;

    /*  If a lookup produced more than one valid result, then the target
     *  range is stored here.  */
    std::pair<itr, itr> todo;

protected:
    const Root& root;
    Dependencies& deps;

    const CellKey target;
    Sheet* sheet;
};

}   // namespace Graph
