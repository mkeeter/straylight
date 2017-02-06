#pragma once

#include "kernel/tree/tree.hpp"

struct s7_scheme;
struct s7_cell;

namespace Kernel {

namespace Bind {

struct Shape {
    Shape(Kernel::Tree t) : tree(t) {}

    Kernel::Tree tree;

    /*  Marks whether the value has changed recently
     *  (which is used to deny equality comparisons)    */
    bool value_changed=false;

    // Type tag for every Shape in s7 interpreter
    static int tag;
};

/*
 *  Inject kernel bindings into the given interpreter
 */
void init(s7_scheme* sc);

/*
 *  Looks up the *env* variable associated with the given tree
 */
std::list<int> envOf(s7_scheme* sc, Kernel::Tree t);

extern "C" {
    bool is_shape(s7_cell* obj);
    const Shape* get_shape(s7_cell* obj);
};

}   // namespace Bind
}   // namespace Kernel
