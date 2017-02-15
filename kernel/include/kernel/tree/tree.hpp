#pragma once

#include <cstdlib>
#include <memory>

#include "glm/vec4.hpp"

#include "kernel/tree/opcode.hpp"
#include "kernel/tree/cache.hpp"

namespace Kernel {

/*
 *  A Tree represents a tree of math expressions.
 *
 *  Trees are implemented as a flyweight with a shared pointer to a
 *  Cache that stores deduplicated tree data  (children, values, etc).
 */
class Tree
{
public:
    /*
     *  Returns a Tree for the given constant
     */
    Tree(float v);

    /*
     *  Returns a token for the given operation
     *
     *  Arguments should be filled in from left to right
     *  (i.e. a must not be null if b is not null)
     *
     *  If the opcode is POW or NTH_ROOT, b must be an integral CONST
     *  (otherwise an assertion will be triggered).
     *  If the opcode is NTH_ROOT, b must be > 0.
     */
    explicit Tree(Opcode::Opcode op, Tree a=Tree(nullptr, 0),
                                     Tree b=Tree(nullptr, 0));

    /*
     *  Constructors for individual axes (non-affine)
     */
    static Tree X() { return Tree(Opcode::VAR_X); }
    static Tree Y() { return Tree(Opcode::VAR_Y); }
    static Tree Z() { return Tree(Opcode::VAR_Z); }

    /*
     *  Returns an AFFINE token (of the form a*x + b*y + c*z + d)
     */
    static Tree affine(float a, float b, float c, float d);

    /*
     *  Returns a new variable with the given value
     */
    static Tree var(float v);

    /*
     *  Returns a new Tree that is a flattened copy of this tree
     */
    Tree collapse() const;

    /*
     *  Attempts to get affine terms from a AFFINE_VEC token
     *  If success is provided, it is populated with true or false
     */
    glm::vec4 getAffine(bool* success=nullptr) const
        { return parent->getAffine(id, success); }

    /*
     *  Accessors for token fields
     */
    Opcode::Opcode opcode() const   { return parent->opcode(id); }
    Tree lhs() const                { return Tree(parent, parent->lhs(id)); }
    Tree rhs() const                { return Tree(parent, parent->rhs(id)); }
    size_t rank() const             { return parent->rank(id); }
    float value() const             { return parent->value(id); }
    Cache::Id var() const
    {
        assert(opcode() == Opcode::VAR);
        return parent->lhs(id);
    }

    uint8_t flags() const           { return parent->flags(id); }

    /*
     *  Sets a VAR tree's value
     *
     *  Note that this may invalidate values of trees that use this variable;
     *  recursively call checkValue to make sure these values are valid.
     */
    void setValue(float v) { return parent->setValue(id, v); }

    /*
     *  Checks that a location-agnostic tree has the correct value
     *
     *  Requires that both children have correct values as well.
     */
    void checkValue();

    /*
     *  Overloaded operators
     */
    Tree operator-() const;

    /*
     *  Comparison operator for trees
     */
    bool operator==(const Tree& other) const {
        return parent.get() == other.parent.get() &&
               id == other.id;
    }

    /*  Bitfield enum for node flags */
    enum Flags {
        /* Has this Id been collapsed?
         * A collapsed Id doesn't contain any magical tokens
         * (e.g. AFFINE) and is ready for evaluation  */
        FLAG_COLLAPSED          = (1<<0),

        /*  Does this Id only contain constants and variables
         *  (no VAR_X, VAR_Y, or VAR_Z opcodes allowed) */
        FLAG_LOCATION_AGNOSTIC  = (1<<1),
    };

    /*
     *  Returns the parent Cache
     */
    const Cache& cache() const { return *parent; }

protected:
    /*
     *  Private constructor
     */
    explicit Tree(std::shared_ptr<Cache> parent, Cache::Id id=0)
        : parent(parent), id(id) {}

    /*  Shared pointer to parent Cache
     *  Every token that refers back to this cache has a pointer to it,
     *  and the Cache is only deleted when all tokens are destroyed     */
    std::shared_ptr<Cache> parent;

    /*  ID indexing into the parent Cache */
    const Cache::Id id;

    /*  An Evaluator needs to be able to pull out the parent Cache */
    friend class EvaluatorBase;
    friend class EvaluatorAVX;

    /*  A Cache needs to construct a raw tree in some cases  */
    friend class Cache;
};

}   // namespace Kernel

// Mass-produce declarations for overloaded operations
#define OP_UNARY(name)      Kernel::Tree name(const Kernel::Tree& a)
OP_UNARY(square);
OP_UNARY(sqrt);
OP_UNARY(abs);
OP_UNARY(sin);
OP_UNARY(cos);
OP_UNARY(tan);
OP_UNARY(asin);
OP_UNARY(acos);
OP_UNARY(atan);
OP_UNARY(exp);
#undef OP_UNARY

#define OP_BINARY(name)     Kernel::Tree name(const Kernel::Tree& a, const Kernel::Tree& b)
OP_BINARY(operator+);
OP_BINARY(operator*);
OP_BINARY(min);
OP_BINARY(max);
OP_BINARY(operator-);
OP_BINARY(operator/);
OP_BINARY(atan2);
OP_BINARY(pow);
OP_BINARY(nth_root);
OP_BINARY(mod);
OP_BINARY(nanfill);
#undef OP_BINARY
