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
     *  Requires that hasItem(k) be true
     */
    CellKey toCellKey(const NameKey& k) const;

    /*
     *  Converts a cell key into a name key
     *  Requires that the cell key be valid
     */
    NameKey toNameKey(const CellKey& k) const;

    /*
     *  Insert a new cell into the graph, re-evaluating as necessary
     */
    CellIndex insertCell(const SheetIndex& parent, const std::string& name,
                         const std::string& expr="");

    /*
     *  Inserts a new instance into the graph
     */
    InstanceIndex insertInstance(const SheetIndex& parent,
                                 const std::string& name,
                                 const SheetIndex& target);

    /*
     *  Erases a cell, triggering re-evaluation
     */
    void eraseCell(const CellIndex& cell);

    /*
     *  Changes a cell's expression, re-evaluating as necessary
     */
    void setExpr(const CellIndex& cell, const std::string& expr);

    /*
     *  Changes the input expression for a particular instance
     */
    void setInput(const InstanceIndex& instance, const CellIndex& cell,
                  const std::string& expr);

    /*
     *  Assigns the given value to a cell
     *  TODO: this is only used in test harnesses
     */
    void setValue(const CellKey& cell, const Value& v);

    /*
     *  Looks up the value of a particular env + cell
     */
    const Value& getValue(const CellKey& cell) const;

    ////////////////////////////////////////////////////////////////////////////
    // Forwarding functions from stored Tree

    /*
     *  Looks up an item by index
     */
    const Item& getItem(const ItemIndex& item) const
        { return tree.at(item); }

    /*
     *  Looks up the name of an item
     */
    const std::string& nameOf(const ItemIndex& item) const
        { return tree.nameOf(item); }

    /*
     *  Check to see whether the given sheet has a particular item
     */
    bool hasItem(const SheetIndex& sheet, const std::string& name) const
        {   return tree.hasItem(sheet, name); }

    /*
     *  Looks up an item by name
     */
    const Item& getItem(const SheetIndex& sheet, const std::string& name) const
        {   return tree.at(sheet, name); }

    /*
     *  Iterate over items belonging to a particular sheet
     */
    const std::list<ItemIndex>& iterItems(const SheetIndex& parent) const
        { return tree.iterItems(parent); }

    /*
     *  Checks to see whether we can insert the given cell
     */
    bool canInsertCell(const SheetIndex& parent,
                       const std::string& name) const
        { return tree.canInsertCell(parent, name); }

    /*
     *  Checks to see whether we can insert the given instance
     *  (checking both name collisions and recursive loops)
     */
    bool canInsertInstance(const SheetIndex& parent, const SheetIndex& target,
                           const std::string& name) const
        { return tree.canInsertInstance(parent, target, name); }

    /*
     *  Check whether we can rename a cell or instance
     */
    bool canRename(const ItemIndex& item, const std::string& new_name) const
        { return tree.canRename(item, new_name); }

    ////////////////////////////////////////////////////////////////////////////

    bool canInsertSheet(const SheetIndex& parent,
                        const std::string& name) const
        { return lib.canInsert(parent, name); }

    SheetIndex insertSheet(const SheetIndex& parent, const std::string& name)
        { return lib.insert(parent, name); }

    ////////////////////////////////////////////////////////////////////////////

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
    void sync();

    /*
     *  Looks up an item by index (non-const version)
     */
    Item& getMutableItem(const ItemIndex& item)
        { return tree.at(item); }

    ////////////////////////////////////////////////////////////////////////////

    /*
     *  Mark a particular key as dirty (if it exists); otherwise, marks
     *  everything that looked at it (the second case matters when we're
     *  deleting items from the graph)
     */
    void markDirty(const NameKey& k);

    /*
     *  Push a particular key to the dirty list, inserting it at the correct
     *  position in the list to minimize re-evaluation.
     */
    void pushDirty(const CellKey& k);

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
