#pragma once

#include "keys.hpp"
#include "cell.hpp"

struct s7_cell;
struct s7_scheme;

namespace Graph
{
struct Dependencies;

struct Interpreter
{
    Interpreter();
    ~Interpreter();

    s7_scheme* const interpreter;

    /*
     *  Evaluates the given cell + environment, logging dependencies
     *  in deps.  Returns true if the result has changed.
     */
    bool eval(const CellKey& key, Dependencies* deps);

    /*
     *  Extracts and returns the default expression for an input cell
     */
    std::string defaultExpr(const Cell* cell) const;

    /*
     *  Checks whether a cell is BASIC, INPUT, or OUTPUT
     */
    Cell::Type cellType(const Cell* cell) const;

    /*
     *  Checks to see whether the given name is valid
     */
    bool nameValid(const std::string& name) const;

    /*
     *  Release interpreter-related storage of the given cell
     */
    void release(Cell* cell);

private:
    /*
     *  Helper functions used in cellType
     */
    bool isInput(const Cell* cell) const;
    bool isOutput(const Cell* cell) const;

    /*
     *  Returns a value thunk for looker looking up lookee
     */
    s7_cell* valueThunk(const CellKey& lookee, const CellKey& looker,
                          Dependencies* deps);

    /*  Scheme function pointers */
    s7_cell* const check_upstream;
    s7_cell* const value_thunk_factory;
    s7_cell* const instance_thunk_factory;
    s7_cell* const eval_func;

    s7_cell* const is_input;
    s7_cell* const is_output;
    s7_cell* const default_expr;
    s7_cell* const name_valid;
};
}   //  namespace Graph
