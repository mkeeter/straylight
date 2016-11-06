#pragma once

#include <memory>
#include <list>

#include "types.hpp"
#include "dependencies.hpp"
#include "interpreter.hpp"
#include "sheet.hpp"
#include "instance.hpp"

namespace Graph
{
struct Cell;

class Root
{
public:
    Root();

    /*
     *  Insert a cell into a particular Sheet
     */
    Cell* insertCell(Sheet* sheet, const Name& name, const Expr& expr);
    void editCell(Cell* cell, const Expr& expr);
    void eraseCell(Cell* cell);

    /*
     *  Edits an instance input expression
     */
    void editInput(Instance* i, Cell* cell, const Expr& expr);

    /*
     *  Insert a new instance to a given Sheet
     */
    Instance* insertInstance(Sheet* sheet, const Name& name, Sheet* target);
    void eraseInstance(Instance* i);

    /*
     *  Checks to see whether we can insert the given sheet
     *  by examining whether this would create a recursive loop.
     */
    bool canInsertInstance(Sheet* sheet, Sheet* target) const;

    /*
     *  Renames a cell or instance
     *  Requires canInsert(sheet, name) to be true
     */
    void rename(Sheet* sheet, const Name& orig, const Name& name);

    /*
     *  Checks whether we can insert a cell or instance
     *  of the given name (since that namespace is shared)
     */
    bool canInsert(Sheet* const sheet, const Name& name) const;

    /*
     *  Create a new Sheet in an existing Sheet's library
     */
    Sheet* createSheet(Sheet* sheet, const Name& name);
    bool canCreateSheet(Sheet* sheet, const Name& name) const;
    void renameSheet(Sheet* sheet, const Name& orig, const Name& name);

    /*
     *  Delete a sheet from a library
     */
    void deleteSheet(Sheet* sheet, const Name& name);

    /*  Encapsulation?  Never heard of it!  */
    std::unique_ptr<Sheet> sheet;
    std::unique_ptr<Instance> instance;

private:
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

    /*
     *  Flushes the dirty buffer, ensuring that everything is up to date
     *  (if frozen is true, then this is a no-op)
     */
    void sync();

    /*
     *  Call when the given item has been changed
     *
     *  This recurses down every instance and marks the given cell or
     *  sheet/name pair as dirty (the second case is used when deleting
     *  or renaming cells).
     */
    void changed(Cell* cell);
    void changed(Sheet* sheet, const Name& name);

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

    /*  struct that stores lookups in both directions  */
    Dependencies deps;

    /*  List of keys that need re-evaluation  */
    std::list<CellKey> dirty;

    /*  When locked, changes don't provoke evaluation
     *  (though the dirty list is still populated)  */
    bool locked=false;

    /*  This is our embedded Scheme interpreter  */
    Interpreter interpreter;
};
}   // namespace Graph
