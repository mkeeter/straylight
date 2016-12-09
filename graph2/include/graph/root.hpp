#pragma once

#include "graph/library.hpp"
#include "graph/interpreter.hpp"
#include "graph/instance.hpp"
#include "graph/dependencies.hpp"
#include "graph/keys.hpp"

class Root
{
public:
    Root() : instance(new Instance(0)), deps(*this)
        { /* Nothing to do here */ }

    /*
     *  Returns true if the given name key points to a cell
     */
    bool isCell(const NameKey& k) const;

    /*
     *  Looks up a name key and converts it to a cell key
     *  Requires that isCell(k) be true
     */
    CellKey toCellKey(const NameKey& k) const;

    /*
     *  Converts a cell key into a name key
     *  Requires that the cell key be valid
     */
    NameKey toNameKey(const CellKey& k) const;

protected:
    /*
     *  Looks up an instance by index
     *  Requires that all indices points to an Instance
     */
    const Instance* getInstance(const std::list<ItemIndex>& i) const;

    /*
     *  RAII-style system for locking the tree
     *  (to prevent intermediate evaluation)
     */
    struct Lock_
    {
        Lock_(Root* r) : was_locked(r->locked), root(r) { r->locked = true; }
        ~Lock_() {
            if (!was_locked)
            {
                root->locked = false;
                root->sync();
            }
        }
        const bool was_locked;
        Root* const root;
    };
    Lock_ Lock() { return Lock_(this); }

    ////////////////////////////////////////////////////////////////////////////

    /*
     *  Flushes the dirty buffer, ensuring that everything is up to date
     *  (if frozen is true, then this is a no-op)
     */
    void sync();

    ////////////////////////////////////////////////////////////////////////////

    /*  Here's all the data in the graph.  Our default sheet is lib[0] */
    std::unique_ptr<Instance> instance;
    Library lib;

    /*  When locked, changes don't provoke evaluation
     *  (though the dirty list is still populated)  */
    bool locked=false;

    /*  This is our embedded Scheme interpreter  */
    Interpreter interpreter;

    /*  struct that stores lookups in both directions  */
    Dependencies deps;

    /*  List of keys that need re-evaluation  */
    std::list<CellKey> dirty;
};
