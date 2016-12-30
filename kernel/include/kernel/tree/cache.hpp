/*
 *  Copyright (C) 2016 Matthew Keeter  <matt.j.keeter@gmail.com>
 *
 *  This file is part of the Ao library.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  Ao is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Ao.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <map>
#include <set>
#include <vector>
#include <thread>
#include <mutex>

#include <boost/bimap.hpp>

#include "glm/vec4.hpp"

#include "kernel/tree/opcode.hpp"

/*
 *  A Cache stores values in a deduplicated math expression
 */
class Cache
{
public:
    /*
     *  Look up the local Cache instance
     *  (which is on a per-thread basis)
     */
    static std::shared_ptr<Cache> instance();
    /*
     *  Forget about the cache for this thread
     */
    static void reset();

    /* Values in the cache are identified by a single value */
    typedef size_t Id;

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
     *  If collapse is true (the default), identity and affine operations will
     *  be collapsed; if false, all branches will be created
     */
    Id operation(Opcode::Opcode op, Id a=0, Id b=0, bool collapse=true);

    /*
     *  Returns a new variable node with the given value
     */
    Id var(float v);

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
    std::set<Id> findConnected(Id root);

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
    float value(Id id) const { return token(id).value(); }

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
        Key(float v, Id id)
          : _Key(0, Opcode::VAR, id, 0, v) { /* Nothing to do here */}

        size_t rank() const             { return std::get<0>(*this); }
        Opcode::Opcode opcode() const   { return std::get<1>(*this); }
        Id lhs() const                  { return std::get<2>(*this); }
        Id var() const                  { return lhs(); }
        Id rhs() const                  { return std::get<3>(*this); }
        float value() const             { return std::get<4>(*this); }
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
    Id next=1;

    /*  Here's the master list of per-thread caches */
    static std::map<std::thread::id, std::shared_ptr<Cache>> instances;
    static std::mutex instance_lock;

    friend class EvaluatorBase;
    friend class EvaluatorAVX;
};
