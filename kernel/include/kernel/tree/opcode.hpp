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
     *  Affine trees must have the form
     *              AFFINE
     *             /      \
     *          ADD         ADD
     *        /    \      /      \
     *      MUL    MUL   MUL     C
     *    /   \   /  \  /   \
     *   X    C  Y   C  Z   C
     *  (where X, Y, Z are base coordinates and C is CONST)
     */
    AFFINE_VEC,

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
}


}   // namespace Kernel
