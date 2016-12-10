#include "graph/tree.hpp"

Tree::~Tree()
{
    for (auto& i : storage)
    {
        i.second.dealloc();
    }
}

/*
 *  Insert a new cell with the given expression
 */
ItemIndex Tree::insertCell(std::string name, std::string expr,
                           SheetIndex parent)
{
    return insert(name, Item(expr), parent);
}

/*
 *  Insert a new instance of the given sheet
 */
ItemIndex Tree::insertInstance(std::string name, SheetIndex sheet,
                               SheetIndex parent)
{
    return insert(name, Item(sheet), parent);
}
