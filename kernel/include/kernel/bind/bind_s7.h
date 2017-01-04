#pragma once

#include "kernel/tree/tree.hpp"

struct s7_scheme;
struct s7_cell;

struct Shape {
    Kernel::Tree tree;
};

extern "C" {
    void kernel_bind_s7(s7_scheme* interpreter);
    bool is_shape(s7_cell* obj);
    const Shape* get_shape(s7_cell* obj);
};
