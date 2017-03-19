#pragma once

#include <cstdlib>
#include <string>

namespace Kernel {

namespace Opcode
{
enum Opcode
{
    INVALID,

    CONST,
    VAR_X,
    VAR_Y,
    VAR_Z,
    VAR,
    CONST_VAR,

    SQUARE,
    SQRT,
    NEG,
    ABS,
    SIN,
    COS,
    TAN,
    ASIN,
    ACOS,
    ATAN,
    EXP,

    ADD,
    MUL,
    MIN,
    MAX,
    SUB,
    DIV,
    ATAN2,
    POW,
    NTH_ROOT,
    MOD,
    NANFILL,

    /*
     *  Dummy opcodes used to select the left or right-hand side of a function
     * (used when one of the children is disabled)
     */
    DUMMY_A,
    DUMMY_B,

    LAST_OP,
};

size_t args(Opcode op);
std::string to_str(Opcode op);
Opcode from_str(std::string s);
bool isCommutative(Opcode op);

}


}   // namespace Kernel
