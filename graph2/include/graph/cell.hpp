#pragma once

#include <string>
#include <map>
#include <list>

#include "graph/node.hpp"
#include "graph/id.hpp"
#include "graph/value.hpp"

namespace Graph {

class Sheet;

class Cell : public Node
{
public:
    enum Type { BASE, INPUT, OUTPUT };
    typedef std::list<InstanceId> Env;

protected:
    /*  Expression to be evaluated  */
    std::string expr;

    /*  Per-environment value list  */
    std::map<Env, Value> values;

    /*  Cell type, which is set by the outside world */
    Type type;
};

}   // namespace Graph
