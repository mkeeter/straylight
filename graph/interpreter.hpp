#pragma once

#include "s7.h"
#include "keys.hpp"

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

    bool isInput(const Cell* cell) { return false; };
    bool isOutput(const Cell* cell) { return false; };
    std::string defaultExpr(const Cell* cell) { return ""; };

    /*
     *  Release interpreter-related storage of the given cell
     */
    void release(Cell* cell);

private:
    /*  Scheme function pointers */
    s7_pointer const check_upstream;
    s7_pointer const value_thunk_factory;
    s7_pointer const eval_func;

    s7_pointer const is_input;
    //s7_pointer const is_output;
    //s7_pointer const default_expr;
};
}   //  namespace Graph
