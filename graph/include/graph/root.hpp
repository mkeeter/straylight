#pragma once

#include <stack>

#include "graph/cell.hpp"
#include "graph/item.hpp"
#include "graph/interpreter.hpp"
#include "graph/instance.hpp"
#include "graph/dependencies.hpp"
#include "graph/serializer.hpp"
#include "graph/keys.hpp"
#include "graph/tree.hpp"

#include "picojson/picojson.h"

namespace Graph {

class Root
{
public:
    Root();

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
    void insertCell(const SheetIndex& parent, const CellIndex& cell,
                    const std::string& name, const std::string& expr);

    /*
     *  Inserts a new instance into the graph
     */
    InstanceIndex insertInstance(const SheetIndex& parent,
                                 const std::string& name,
                                 const SheetIndex& target);
    void insertInstance(const SheetIndex& parent,
                        const InstanceIndex& instance,
                        const std::string& name,
                        const SheetIndex& target);

    /*
     *  Erases a cell, triggering re-evaluation
     */
    void eraseCell(const CellIndex& cell);

    /*
     *  Erases an instance, triggering re-evaluation
     */
    void eraseInstance(const InstanceIndex& instance);

    /*
     *  Changes a cell's expression, re-evaluating as necessary
     */
    bool setExpr(const CellIndex& cell, const std::string& expr);

    /*
     *  Changes the input expression for a particular instance
     */
    bool setInput(const InstanceIndex& instance, const CellIndex& cell,
                  const std::string& expr);

    /*
     *  Assigns the given value to a cell
     *  TODO: this is only used in test harnesses
     */
    void setValue(const CellKey& cell, const Value& v);

    /*
     *  Looks up the value of a particular env + cell
     *  TODO: this is only used in test harnesses?
     */
    const Value& getValue(const CellKey& cell) const;

    /*
     *  Checks to see whether the given sheet name is valid
     *  (both as an identifier and in having no collisions)
     */
    bool checkItemName(const SheetIndex& parent,
                       const std::string& name, std::string* err=nullptr) const;

    /*
     *  Renames an item, resynching anything that looked for it
     */
    void renameItem(const ItemIndex& i, const std::string& name);

    /*
     *  Exports the graph to any object implementing the TreeSerializer API
     *  This is commonly used to render into a UI in a immediate style
     */
    void serialize(TreeSerializer* s) const;

    /*
     *  Encode the entire graph in JSON
     *
     *  This is used to save files to disk with an appropriate encoder
     */
    std::string toString() const;

    /*
     *  Deserializes from string (which should be JSON)
     *
     *  The existing graph is cleared.
     *  Returns an error string on failure or the empty string on success
     */
    std::string fromString(const std::string& str);

    /*
     *  Removes all items from the graph
     */
    void clear();

    /*
     *  Checks to see whether the given sheet name is valid
     *
     *  If err is given, it is populated with an error message
     */
    bool checkSheetName(const SheetIndex& parent,
                        const std::string& name, std::string* err=nullptr) const;

    bool canInsertSheet(const SheetIndex& parent,
                        const std::string& name) const
        { return checkSheetName(parent, name); }

    SheetIndex insertSheet(const SheetIndex& parent, const std::string& name);
    void insertSheet(const SheetIndex& parent, const SheetIndex& sheet,
                     const std::string& name);

    /*
     *  Renames a sheet
     */
    void renameSheet(const SheetIndex& i, const std::string& name)
        { tree.rename(i, name); }

    /*
     *  Erases a sheet and any instances thereof
     */
    void eraseSheet(const SheetIndex& s);

    /*
     *  Returns all of the sheets above (contained within) the given env
     *
     *  Note that this doesn't tell us whether we can insert an instance
     *  of these sheets, as this could create a recursive loop; use
     *  tree.canInsertInstance to check.
     */
    std::list<SheetIndex> sheetsAbove(const Env& e) const;

    /*
     *  Temporarily builds up the given sheet, setting the given inputs
     *
     *  Returns the sheet's IO values (wrapped); sets *err if an error occurs
     */
    std::map<std::string, Value> callSheet(
            const CellKey& caller, const SheetIndex& sheet,
            const std::list<Value> inputs, std::string* err=nullptr);

    ////////////////////////////////////////////////////////////////////////////

    /*
     *  Calls a function with the interpreter pointer
     */
    void call(void (*f)(s7_scheme*)) { interpreter.call(f); }
    std::set<std::string> keywords() const { return interpreter.keywords(); }

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

    /*  Get const reference to our internally mutable tree */
    const Tree& getTree() const { return tree; }

protected:
    /*
     *  Flushes the dirty buffer, ensuring that everything is up to date
     *  (if frozen is true, then this is a no-op)
     */
    void sync();

    ////////////////////////////////////////////////////////////////////////////

    /*
     *  Checks to see whether the given env is valid
     *  (i.e. that every item in it exists and is an instance)
     */
    bool checkEnv(const Env& env) const;

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

    /*
     *  Exports the graph to any object implementing the TreeSerializer API
     */
    void serialize(TreeSerializer* s, const Env& env) const;

    /*
     *  Export the given sheet to JSON (recursively)
     */
    picojson::value toJson(SheetIndex sheet) const;

    /*
     *  Deserializes a particular sheet (recursively)
     *
     *  Returns an error string on error, empty string otherwise
     */
    std::string fromJson(SheetIndex sheet, const picojson::value& value);

    ////////////////////////////////////////////////////////////////////////////

    /*  Here's all the data in the graph.  */
    /*  This is our mutable tree; we also define a const reference to it
     *  named tree that is accessible for others to read */
    Tree tree;

    /*  When locked, changes don't provoke evaluation
     *  (though the dirty list is still populated)  */
    bool locked=false;

    /*  struct that stores lookups in both directions  */
    Dependencies deps;

    /*  This is our embedded Scheme interpreter  */
    Interpreter interpreter;

    /*  List of keys that need re-evaluation  */
    std::stack<std::list<CellKey>> dirty;
};

}   // namespace Graph
