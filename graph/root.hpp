#pragma once

#include <memory>
#include <list>

#include "types.hpp"
#include "dependencies.hpp"

namespace Graph
{
struct Sheet;
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
     *  Find the parent sheet of a cell
     *  Returns nullptr if the cell has no parent in the graph library
     */
    Sheet* parentSheet(Cell* cell) const;

    /*
     *  Insert a new instance to a given Sheet
     */
    Instance* insertInstance(Sheet* sheet, const Name& name, Sheet* target);

    /*
     *  Renames a cell or instance
     *  Requires canInsert(sheet, name) to be true
     */
    void rename(Sheet* sheet, const Name& orig, const Name& name);

    /*
     *  Checks whether we can insert a cell or instance
     *  of the given name (since that namespace is shared)
     */
    bool canInsert(Sheet* sheet, const Name& name);

    /*
     *  Create a new Sheet in an existing Sheet's library
     */
    Sheet* createSheet(Sheet* sheet, const Name& name);
    bool canCreateSheet(Sheet* sheet, const Name& name);

    /*
     *  Erase a cell or instance from a Sheet
     */
    void erase(Sheet* sheet, const Name& name);

    /*
     *  Delete a sheet from a library
     */
    void deleteSheet(Sheet* sheet, const Name& name);

private:
    /*
     *  RAII-style system for locking the tree
     *  (to prevent intermediate evaluation)
     */
    struct Lock_
    {
        Lock_(Root* r) : was_locked(r->locked), root(r) {}
        ~Lock_() {
            if (was_locked)
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
     *  Evaluates the given cell + environment
     *  Returns true if the result has changed
     */
    bool eval(const CellKey& k);

    /*
     *  Flushes the dirty buffer, ensuring that everything is up to date
     *  (if frozen is true, then this is a no-op)
     */
    void sync();

    /*
     *  Call when the given item has been changed
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

    std::unique_ptr<Sheet> sheet;
    std::unique_ptr<Instance> instance;

    /*  struct that stores lookups in both directions  */
    Dependencies deps;

    /*  List of keys that need re-evaluation  */
    std::list<CellKey> dirty;

    /*  When locked, changes don't provoke evaluation
     *  (though the dirty list is still populated)  */
    bool locked=false;
};
}   // namespace Graph
