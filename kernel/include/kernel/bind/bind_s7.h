#pragma once

#include "kernel/tree/tree.hpp"

struct s7_scheme;

extern "C" {
    void kernel_bind_s7(s7_scheme* interpreter);

    struct Shape {
        Kernel::Tree tree;
    };
};
