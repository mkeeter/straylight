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
    Interpreter();
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

private:
    /*
     *  Helper functions used in cellType
     */
    bool isInput(const std::string& expr) const;
    bool isOutput(const std::string& expr) const;

    /*  This is the main interpreter process  */
    s7_scheme* const interpreter;

    /*  Scheme function pointers */
    s7_cell* const is_input;
    s7_cell* const is_output;
    s7_cell* const default_expr;
    s7_cell* const name_valid;
};
