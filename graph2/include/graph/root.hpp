#pragma once

#include "graph/library.hpp"
#include "graph/cell.hpp"
#include "graph/item.hpp"
#include "graph/interpreter.hpp"
#include "graph/instance.hpp"
#include "graph/dependencies.hpp"
#include "graph/keys.hpp"
#include "graph/tree.hpp"

class Root
{
public:
    Root() : instance(new Instance(0)), deps(*this), interpreter(*this, &deps)
        { /* Nothing to do here */ }

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

    /*
     *  Returns every environment in which the given sheet is instanced
     */
    std::list<Env> envsOf(const SheetIndex& i) const;

    /*
     *  Looks up an item by index
     */
    const Item& getItem(const ItemIndex& item) const
        { return tree.at(item); }

    /*
     *  Look up a sheet by index
     */
    const Sheet& getSheet(const SheetIndex& sheet) const
        { return lib.at(sheet); }

    /*
     *  Insert a new cell into the graph, re-evaluating as necessary
     */
    ItemIndex insertCell(const SheetIndex& parent, const std::string& name,
                         const std::string& expr="");

    /*
     *  Changes a cell's expression, re-evaluating as necessary
     */
    void setExpr(const ItemIndex& cell, const std::string& expr);

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

protected:
    /*
     *  Flushes the dirty buffer, ensuring that everything is up to date
     *  (if frozen is true, then this is a no-op)
     */
    void sync() { /* TODO */ }

    /*
     *  Looks up an item by index (non-const version)
     */
    Item& getMutableItem(const ItemIndex& item)
        { return tree.at(item); }

    ////////////////////////////////////////////////////////////////////////////

    /*  Here's all the data in the graph.  Our default sheet is lib[0] */
    Tree tree;
    std::unique_ptr<Instance> instance;
    Library lib;

    /*  When locked, changes don't provoke evaluation
     *  (though the dirty list is still populated)  */
    bool locked=false;

    /*  struct that stores lookups in both directions  */
    Dependencies deps;

    /*  This is our embedded Scheme interpreter  */
    Interpreter interpreter;

    /*  List of keys that need re-evaluation  */
    std::list<CellKey> dirty;
};
