#include "s7/s7.h"

#include "kernel/bind/bind_s7.h"

////////////////////////////////////////////////////////////////////////////////

// Define custom types, etc here

////////////////////////////////////////////////////////////////////////////////

void kernel_bind_s7(s7_scheme* interpreter)
{
    (void)interpreter;
    printf("Injecting kernel bindings into interpreter\n");
    // Install custom types here
}
