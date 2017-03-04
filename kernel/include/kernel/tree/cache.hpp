#pragma once

#include <map>
#include <set>
#include <vector>
#include <thread>
#include <mutex>

#include <boost/bimap.hpp>

#include "glm/vec4.hpp"

#include "kernel/tree/opcode.hpp"

namespace Kernel {

/*
 *  A Cache stores values in a deduplicated math expression
 */
class Cache
{
protected:
    /*
     *  Strongly-typed id for items in the cache
     */
    template<typename T>
    struct Id_ {
        Id_(unsigned i): i(i) { /* Nothing to do here */ }
        bool operator<(const Id_& other) const  { return i < other.i; }
        bool operator==(const Id_& other) const { return i == other.i; }
        bool operator!=(const Id_& other) const { return !(i == other.i); }

        unsigned i;
    };

    struct Base_ {};
    struct Var_ {};

public:
    typedef Id_<Var_> VarId;
    class Id : public Id_<Base_>
    {
    public:
        // VarId can decay to Id implicitly
        Id(const VarId& other) : Id_<Base_>(other.i) {}
        Id(unsigned i) : Id_<Base_>(i) {}
    };

    /*
     *  Look up the local Cache instance
     *  (which is on a per-thread basis)
     */
    static std::shared_ptr<Cache> instance();
    /*
     *  Forget about the cache for this thread
     */
    static void reset();

    /*
     *  Returns a token for the given constant
     */
    Id constant(float v);

    /*
     *  Returns a token for the given operation
     *
     *  Arguments should be filled in from left to right
     *  (i.e. a must not be null if b is not null)
     *
     *  If simplify is true (the default), identity and affine operations will
     *  be simplified; if false, all branches will be created
     */
    Id operation(Opcode::Opcode op, Id a=0, Id b=0, bool simplify=true);

    /*
     *  Returns a new variable node with the given value
     */
    VarId var(float v);

    Id X() { return operation(Opcode::VAR_X); }
    Id Y() { return operation(Opcode::VAR_Y); }
    Id Z() { return operation(Opcode::VAR_Z); }

    /*
     *  Returns an AFFINE token (of the form a*x + b*y + c*z + d)
     */
    Id affine(float a, float b, float c, float d);
    Id affine(glm::vec4 v) { return affine(v.x, v.y, v.z, v.w); }

    /*
     *  Finds every token descending from root
     */
    std::set<Id> findConnected(Id root) const;

    /*
     *  If the given Id is an AFFINE_VEC, return the affine terms
     *
     *  Set success to true / false if it is provided
     */
    glm::vec4 getAffine(Id root, bool* success=nullptr) const;

    /*
     *  Collapses AFFINE nodes into normal Opcode::ADD, taking advantage of
     *  identity operations to make the tree smaller.
     *
     *  All Ids remain valid.
     *
     *  Returns a root token for the new tree.
     */
    Id collapse(Id root);

    /*
     *  Accessor functions for token fields
     */
    Opcode::Opcode opcode(Id id) const { return token(id).opcode(); }
    Id lhs(Id id) const { return token(id).lhs(); }
    Id rhs(Id id) const { return token(id).rhs(); }
    size_t rank(Id id) const { return token(id).rank(); }
    float value(Id id) const { return values.at(id); }

    /*
     *  Looks up flags for a node.  The id must exist in this cache.
     */
    uint8_t flags(Id id) const { return flags_.at(id); }

    /*
     *  Sets the value for a particular node
     *
     *  The node must be location-agnostic, but doesn't necessarily
     *  need to be a variable node.
     */
    void setValue(Id id, float v);

    /*
     *  Returns all the variable values underneath the given node
     */
    std::map<Kernel::Cache::VarId, float> vars(Id id) const;

protected:
    /*
     *  Cache constructor is private so outsiders must use instance()
     */
    Cache() {}

    /*
     *  Checks whether the operation is an identity operation
     *  If so returns an appropriately simplified tree
     *  i.e. (X + 0) will return X
     */
    Id checkIdentity(Opcode::Opcode op, Id a, Id b);

    /*
     *  Checks whether the operation should be handled as an affine
     *  transformation, returning an AFFINE tree if true.
     */
    Id checkAffine(Opcode::Opcode op, Id a, Id b);

    /*
     *  If the opcode is commutative, consider tweaking tree structure
     *  to keep it as balanced as possible.
     */
    Id checkCommutative(Opcode::Opcode op, Id a, Id b);

    /*
     *  Rebuilds a tree from the base up, returning the new root
     *
     *  All old Cache::Ids remain valid, though they may be orphaned in
     *  the tree
     */
    Id rebuild(Id root, std::map<Id, Id> changed);

    /*
     *  Keys store all relevant token data
     *
     *  tuples are compared lexigraphically, so we putting rank first
     *  means that iterating over the bimap happens in rank order
     */
    typedef std::tuple<size_t,  /* rank */
                       Opcode::Opcode, /* opcode */
                       Id, /* lhs */
                       Id, /* rhs */
                       float    /* value (for constants) */> _Key;
    class Key : public _Key
    {
    public:
        Key(float v)
          : _Key(0, Opcode::CONST, 0, 0, v) { /* Nothing to do here */ }
        Key(Opcode::Opcode op, Id a, Id b, size_t rank)
          : _Key(rank, op, a, b, 0.0f) { /* Nothing to do here */}
        Key(Id id)
          : _Key(0, Opcode::VAR, id, 0, 0) { /* Nothing to do here */}

        size_t rank() const             { return std::get<0>(*this); }
        Opcode::Opcode opcode() const   { return std::get<1>(*this); }
        Id lhs() const                  { return std::get<2>(*this); }
        Id rhs() const                  { return std::get<3>(*this); }
        float value() const             { return std::get<4>(*this); }

        VarId var() const
        {
            assert(opcode() == Opcode::VAR);
            return VarId(lhs().i);
        }
    };

    /*
     *  Key constructors
     */
    Key key(float v) const;
    Key key(Opcode::Opcode op, Id a, Id b) const;

    /*
     *  Id reverse lookup
     */
    Key token(Id id) const { return data.right.at(id); }

    boost::bimap<Key, Id> data;

    /*  For non-constants, values are stored in this separate array
     *  (to keep the data array immutable)  */
    std::map<Id, float> values;

    unsigned next=1;

    /*  Per-Id flags
     *  These could be deterministically calculated from the state
     *  of the Cache, but it is less expensive to calculate them once per
     *  Id and store them here.  This should be a bitwise Or of Flags */
    std::map<Id, uint8_t> flags_;

    /*  Here's the master list of per-thread caches */
    static std::map<std::thread::id, std::shared_ptr<Cache>> instances;
    static std::mutex instance_lock;

    friend class EvaluatorBase;
    friend class EvaluatorAVX;
};

}   // namespace Kernel

////////////////////////////////////////////////////////////////////////////////

// #[derive(Hash)]
namespace std
{
    template <>
    struct hash<Kernel::Cache::Id>
    {
        size_t operator()(const Kernel::Cache::Id& k) const
        {
            return hash<unsigned>()(k.i);
        }
    };
}
