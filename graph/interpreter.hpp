#pragma once

#include "s7.h"

#include "keys.hpp"
#include "cell.hpp"

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

    /*  Scheme function pointers */
    s7_pointer const check_upstream;
    s7_pointer const value_thunk_factory;
    s7_pointer const instance_thunk_factory;
    s7_pointer const eval_func;

    s7_pointer const is_input;
    s7_pointer const is_output;
    s7_pointer const default_expr;
    s7_pointer const name_valid;
};
}   //  namespace Graph
