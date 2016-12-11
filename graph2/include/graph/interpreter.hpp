#pragma once

#include "graph/cell.hpp"
#include "graph/keys.hpp"

struct s7_cell;
struct s7_scheme;

class Dependencies;
class Root;

class Interpreter
{
public:
    Interpreter(const Root& parent);
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
    Value eval(const CellKey& key, Dependencies* deps);

    /*
     *  Releases the given value's pointer
     */
    void release(ValuePtr v);

    static CellKey decodeCellKey(s7_scheme* interpreter, s7_cell* v);
    static NameKey decodeNameKey(s7_scheme* interpreter, s7_cell* v);

private:
    /*
     *  Helper functions used in cellType
     */
    bool isInput(const std::string& expr) const;
    bool isOutput(const std::string& expr) const;

    s7_cell* encodeCellKey(const CellKey& k) const;
    s7_cell* encodeNameKey(const NameKey& k) const;
    CellKey decodeCellKey(s7_cell* v) const
        { return decodeCellKey(interpreter, v); }
    NameKey decodeNameKey(s7_cell* v) const
        { return decodeNameKey(interpreter, v); }

    /*
     *  Returns a thunk that looks up a value (for a cell) or an output value
     *  (for an instance)
     */
    s7_cell* getThunk(const Env& env, const ItemIndex& item,
                      const CellKey& caller, Dependencies* deps);

    /*  Reference to graph root  */
    const Root& root;

    /*  This is the main interpreter process  */
    s7_scheme* const interpreter;

    /*  Scheme function pointers */
    s7_cell* const is_input;
    s7_cell* const is_output;
    s7_cell* const default_expr;
    s7_cell* const name_valid;

    s7_cell* const check_upstream;
    s7_cell* const value_thunk_factory;
    s7_cell* const instance_thunk_factory;
    s7_cell* const eval_func;
};
