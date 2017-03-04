#pragma once

#include "kernel/tree/tree.hpp"

struct s7_scheme;
struct s7_cell;

namespace Kernel {

namespace Bind {

struct shape_t {
    shape_t(Kernel::Tree t) : tree(t) {}

    Kernel::Tree tree;
    std::map<Kernel::Cache::VarId, float> vars;

    // Type tag for every shape_t in s7 interpreter
    static int tag;
};

/*
 *  Inject kernel bindings into the given interpreter
 */
void init(s7_scheme* sc);

extern "C" {
    /*
     *  Turns the given Tree into a shape
     */
    s7_cell* shape_new(s7_scheme* sc, Kernel::Tree t);

    /*
     *  Checks if the given Tree is a shape
     */
    bool is_shape(s7_cell* obj);

    /*
     *  Converts a cell pointer into a shape pointer
     */
    const shape_t* get_shape(s7_cell* obj);
};

}   // namespace Bind
}   // namespace Kernel
