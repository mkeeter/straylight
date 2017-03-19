#include <algorithm>
#include <set>
#include <list>
#include <cmath>
#include <cassert>

#include "kernel/tree/cache.hpp"

namespace Kernel {

Tree::Tree()
    : ptr(nullptr)
{
    // Nothing to do here
}

Tree::Tree(float v)
    : ptr(Cache::instance()->constant(v))
{
    // Nothing to do here
}

Tree::Tree(Opcode::Opcode op, Tree a, Tree b)
    : ptr(Cache::instance()->operation(op, a.ptr, b.ptr))
{
    // Aggressive sanity-checking
    assert((Opcode::args(op) == 0 && a.ptr.get() == 0 && b.ptr.get() == 0) ||
           (Opcode::args(op) == 1 && a.ptr.get() != 0 && b.ptr.get() == 0) ||
           (Opcode::args(op) == 2 && a.ptr.get() != 0 && b.ptr.get() != 0));

    // POW only accepts integral values as its second argument
    if (op == Opcode::POW)
    {
        assert(b->op == Opcode::CONST &&
               b->value == std::round(b->value));
    }
    else if (op == Opcode::NTH_ROOT)
    {
        assert(b->op == Opcode::CONST &&
               b->value == std::round(b->value) &&
               b->value > 0);
    }
}

Tree Tree::var()
{
    return Tree(Cache::instance()->var());
}

Tree::Tree_::~Tree_()
{
    if (op == Opcode::CONST)
    {
        Cache::instance()->del(value);
    }
    else if (op != Opcode::VAR)
    {
        Cache::instance()->del(op, lhs, rhs);
    }
}

Tree Tree::remap(Tree X_, Tree Y_, Tree Z_) const
{
    std::list<std::shared_ptr<Tree::Tree_>> tape;

    {   // Flatten the tree into a tape in reverse-weighted order
        std::set<Tree::Tree_*> found = {nullptr};
        std::list<std::shared_ptr<Tree::Tree_>> todo = { ptr };
        while (todo.size())
        {
            auto t = todo.front();
            todo.pop_front();

            if (found.find(t.get()) == found.end())
            {
                todo.push_back(t->lhs);
                todo.push_back(t->rhs);
                tape.push_front(t);
            }
        }
    }

    std::map<Tree::Tree_*, std::shared_ptr<Tree_>> m = {
        {X().id(), X_.ptr}, {Y().id(), Y_.ptr}, {Z().id(), Z_.ptr}};

    for (const auto& t : tape)
    {
        auto lhs = m.find(t->lhs.get());
        auto rhs = m.find(t->rhs.get());
        if (lhs != m.end() || rhs != m.end())
        {
            m.insert({t.get(), Cache::instance()->operation(t->op, lhs->second, rhs->second)});
        }
    }

    // If this Tree was remapped, then return it; otherwise return itself
    auto r = m.find(id());
    return r == m.end() ? *this : Tree(r->second);
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
