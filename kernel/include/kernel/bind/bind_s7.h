#pragma once

#include "kernel/tree/tree.hpp"

struct s7_scheme;
struct s7_cell;

namespace Kernel {

namespace Bind {

struct Shape {
    Kernel::Tree tree;
};

extern "C" {
    void init(s7_scheme* interpreter);
    bool is_shape(s7_cell* obj);
    const Shape* get_shape(s7_cell* obj);
};

}   // namespace Bind
}   // namespace Kernel
