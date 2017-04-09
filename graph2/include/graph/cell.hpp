#pragma once

#include <string>
#include <map>
#include <list>

#include "graph/node.hpp"
#include "graph/id.hpp"
#include "graph/value.hpp"

namespace Graph {

struct Cell : public Node
{
    Cell(SheetId parent) : Node(parent) { /* nothing to do here */ }

    enum Type { BASE, INPUT, OUTPUT };
    typedef std::list<InstanceId> Env;

    /*  Expression to be evaluated  */
    std::string expr;

    /*  Per-environment value list  */
    std::map<Env, Value> values;

    /*  Cell type, which is set by the outside world */
    Type type;
};

}   // namespace Graph
