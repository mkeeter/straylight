#pragma once

#include "graph/library.hpp"
#include "graph/interpreter.hpp"
#include "graph/dependencies.hpp"

struct Sheet;
struct Instance;
struct CellKey;

class Root
{

protected:
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
