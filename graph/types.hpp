#pragma once

#include <string>
#include <vector>

#include "s7.h"

namespace Graph
{
struct Instance;

typedef std::string Name;
typedef std::vector<Name> Id;
typedef std::vector<Instance*> Env;
typedef std::string Expr;
}   // namespace Graph
