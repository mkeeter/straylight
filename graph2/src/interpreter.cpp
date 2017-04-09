#include <cstdlib>

#include "s7/s7.h"
#include "graph/interpreter.hpp"

namespace Graph {

Interpreter::Interpreter()
    : sc(s7_init())
{
    // Nothing to do here
}

};
