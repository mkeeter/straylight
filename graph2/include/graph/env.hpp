#pragma once

#include <vector>

#include "graph/index.hpp"

/*  An env represents a unique path from the root to a particular Sheet,
 *  as a series of indices into the Sheet.items arrays */
typedef std::vector<ItemIndex> Env;
