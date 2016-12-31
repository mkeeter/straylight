#pragma once

struct s7_scheme;

extern "C" {
    void kernel_bind_s7(s7_scheme* interpreter);
};
