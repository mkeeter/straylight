#pragma once

#include "kernel/tree/tree.hpp"

struct s7_scheme;
struct s7_cell;

namespace Kernel {

namespace Bind {

struct Shape {
    Kernel::Tree tree;

    // Type tag for every Shape in s7 interpreter
    static int tag;
};

/*
 *  Inject kernel bindings into the given interpreter
 */
void init(s7_scheme* sc);

extern "C" {
    bool is_shape(s7_cell* obj);
    const Shape* get_shape(s7_cell* obj);
};

}   // namespace Bind
}   // namespace Kernel
