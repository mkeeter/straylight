#pragma once

#include <string>

struct s7_cell;
struct s7_scheme;

namespace Graph {

class SymbolTable;

/*
 *  The Interpreter class handles our embedded Scheme interpreter
 *  It is the only class that knows anything about the embedded system;
 *  everyone else should use the Interpreter::Value type and other
 *  wrapped functions (rather than calling s7_*)
 */
class Interpreter
{
public:
    Interpreter();

    /*  Helpful typedef for values  */
    typedef s7_cell* Value;

    /*
     *  Evaluates a particular string given a symbol table
     *  (which includes information about the target cell)
     *
     *  If the result is unchanged, return a Value with value = nullptr
     */
    Value eval(const std::string& str, SymbolTable& symbols);

protected:
    /*  This is the main interpreter process  */
    s7_scheme* const sc;
};

}   // namespace Graph
