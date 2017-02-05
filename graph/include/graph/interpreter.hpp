#pragma once

#include <set>

#include "graph/cell.hpp"
#include "graph/keys.hpp"

struct s7_cell;
struct s7_scheme;

namespace Graph {

class Root;
class Dependencies;

class Interpreter
{
public:
    Interpreter(Root& parent, Dependencies* deps);
    ~Interpreter();

    /*
     *  Extracts and returns the default expression for an input cell
     */
    std::string defaultExpr(const std::string& expr) const;

    /*
     *  Checks whether a cell is BASIC, INPUT, or OUTPUT
     */
    Cell::Type cellType(const std::string& expr) const;

    /*
     *  Checks to see whether the given name is valid
     */
    bool nameValid(const std::string& name) const;

    /*
     *  Evaluates a in the given environment
     *  If the result is unchanged, returns a Value with value = nullptr
     */
    Value eval(const CellKey& key);

    /*
     *  Releases the given value's pointer
     */
    void release(ValuePtr v);

    /*
     *  Returns an untagged value pointer
     *  e.g. taking '(value 12) and returning 12
     */
    static ValuePtr untag(ValuePtr v);

    /*
     *  Installs a reader macro
     *  The reader macro is evaluated after things like 'output'
     *  are stripped and before 'begin is prepended to the cells
     */
    void setReader(s7_cell* r);

    /*
     *  Checks to see if the given string is reserved
     */
    bool isReserved(const std::string& k) const;

    /*
     *  Calls a function with the interpreter
     *  Used to install custom bindings
     */
    template<typename T>
    T call(T (*f)(s7_scheme*)) { return f(sc); }

    /*
     *  Returns a set of keywords
     */
    std::set<std::string> keywords() const;

private:
    /*
     *  Helper functions used in cellType
     */
    bool isInput(const std::string& expr) const;
    bool isOutput(const std::string& expr) const;

    /*
     *  Returns a thunk that looks up a value (for a cell) or an output value
     *  (for an instance)
     */
    s7_cell* getItemThunk(const Env& env, const ItemIndex& item,
                          const CellKey& caller);

    /*
     *  Returns a thunk that calls a sheet
     */
    s7_cell* getSheetThunk(const SheetIndex& index,
                           const CellKey& looker);

    /*  Reference to graph root  */
    Root& root;
    Dependencies* deps;

    /*  This is the main interpreter process  */
    s7_scheme* const sc;

    /*  Scheme function pointers */
    s7_cell* const is_input;
    s7_cell* const is_output;
    s7_cell* const default_expr;
    s7_cell* const name_valid;

    s7_cell* const eval_func;
    s7_cell* const default_reader;
    s7_cell* reader=nullptr;
};

}   // namespace Graph
