#include <algorithm>
#include <cmath>
#include <cassert>

#include "kernel/tree/tree.hpp"
#include "kernel/eval/evaluator.hpp"

namespace Kernel {

Tree::Tree(float v)
    : Tree(Cache::instance(), Cache::instance()->constant(v))
{
    // Nothing to do here
}

Tree::Tree(Opcode::Opcode op, Tree a, Tree b)
    : Tree(Cache::instance(), Cache::instance()->operation(op, a.id, b.id))
{
    assert(!a.id || a.parent == Cache::instance());
    assert(!b.id || b.parent == Cache::instance());
    assert((Opcode::args(op) == 0 && !a.id && !b.id) ||
           (Opcode::args(op) == 1 &&  a.id && !b.id) ||
           (Opcode::args(op) == 2 &&  a.id &&  b.id));

    // POW only accepts integral values as its second argument
    if (op == Opcode::POW)
    {
        assert(b.opcode() == Opcode::CONST &&
               b.value() == std::round(b.value()));
    }
    else if (op == Opcode::NTH_ROOT)
    {
        assert(b.opcode() == Opcode::CONST &&
               b.value() == std::round(b.value()) &&
               b.value() > 0);
    }
}

/*
 *  Returns an AFFINE token (of the form a*x + b*y + c*z + d)
 */
Tree Tree::affine(float a, float b, float c, float d)
{
    auto s = Cache::instance();
    return Tree(s, s->affine(a, b, c, d));
}

Tree Tree::var(float v)
{
    auto s = Cache::instance();
    return Tree(s, s->var(v));
}

Tree Tree::collapse() const
{
    return (flags() & FLAG_COLLAPSED) ? *this :
           Tree(parent, parent->collapse(id));
}

void Tree::checkValue()
{
    assert((opcode() != Opcode::VAR) &&
           (flags() & Tree::FLAG_LOCATION_AGNOSTIC));

    auto t = Tree(opcode(), Tree(lhs().value()),
                            Tree(rhs().value()));
    Evaluator e(t);
    setValue(e.values(1)[0]);
}

}   // namespace Kernel

////////////////////////////////////////////////////////////////////////////////

// Mass-produce definitions for overloaded operations
#define OP_UNARY(name, opcode) \
Kernel::Tree name(const Kernel::Tree& a) { return Kernel::Tree(opcode, a); }
OP_UNARY(square,    Kernel::Opcode::SQUARE);
OP_UNARY(sqrt,      Kernel::Opcode::SQRT);
Kernel::Tree Kernel::Tree::operator-() const
    { return Kernel::Tree(Kernel::Opcode::NEG, *this); }
OP_UNARY(abs,       Kernel::Opcode::ABS);
OP_UNARY(sin,       Kernel::Opcode::SIN);
OP_UNARY(cos,       Kernel::Opcode::COS);
OP_UNARY(tan,       Kernel::Opcode::TAN);
OP_UNARY(asin,      Kernel::Opcode::ASIN);
OP_UNARY(acos,      Kernel::Opcode::ACOS);
OP_UNARY(atan,      Kernel::Opcode::ATAN);
OP_UNARY(exp,       Kernel::Opcode::EXP);
#undef OP_UNARY

#define OP_BINARY(name, opcode) \
Kernel::Tree name(const Kernel::Tree& a, const Kernel::Tree& b) \
    { return Kernel::Tree(opcode, a, b); }
OP_BINARY(operator+,    Kernel::Opcode::ADD);
OP_BINARY(operator*,    Kernel::Opcode::MUL);
OP_BINARY(min,          Kernel::Opcode::MIN);
OP_BINARY(max,          Kernel::Opcode::MAX);
OP_BINARY(operator-,    Kernel::Opcode::SUB);
OP_BINARY(operator/,    Kernel::Opcode::DIV);
OP_BINARY(atan2,        Kernel::Opcode::ATAN2);
OP_BINARY(pow,          Kernel::Opcode::POW);
OP_BINARY(nth_root,     Kernel::Opcode::NTH_ROOT);
OP_BINARY(mod,          Kernel::Opcode::MOD);
OP_BINARY(nanfill,      Kernel::Opcode::NANFILL);
#undef OP_BINARY
