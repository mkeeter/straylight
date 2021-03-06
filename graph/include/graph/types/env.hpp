#pragma once

#include <list>

#include "graph/types/index.hpp"

namespace Graph {

/*  An env represents a unique path from the root to a particular Sheet,
 *  as a series of indices into the Sheet.items arrays */
typedef std::list<InstanceIndex> Env;

}   // namespace Graph
