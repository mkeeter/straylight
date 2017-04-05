#pragma once

#include <stack>
#include <regex>

#include "graph/root.hpp"
#include "graph/dependencies.hpp"
#include "graph/interpreter.hpp"

namespace Graph {

class Translator;

class Graph
{
public:
    Graph();

    bool isItemName(const std::string& symbol);
    bool isSheetName(const std::string& symbol);

protected:

    /*  Here's all the data in the graph.  */
    Root root;

    /*  When locked, changes don't provoke evaluation
     *  (though the dirty list is still populated)  */
    bool locked=false;

    /*  struct that stores lookups in both directions  */
    Dependencies deps;

    /*  This is our embedded Scheme interpreter  */
    Interpreter interpreter;

    /*  List of keys that need re-evaluation  */
    std::stack<std::list<CellKey>> dirty;

    // Regex strings aren't stored, so we store them statically here
    static const std::list<std::pair<std::string, std::string>> _bad_item_names;
    static const std::list<std::pair<std::string, std::string>> _bad_sheet_names;
    static const std::string _good_item_name;
    static const std::string _good_sheet_name;

    std::list<std::pair<std::regex, std::string>> bad_item_names;
    std::list<std::pair<std::regex, std::string>> bad_sheet_names;
    std::regex good_item_name;
    std::regex good_sheet_name;
};

}   // namespace Graph

