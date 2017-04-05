#pragma once

#include "graph/id.hpp"

namespace Graph {

/*
 *  A Node is a Sheet, Cell, or Instance
 *  It knows the SheetId of its parent, and not much else
 */
struct Node
{
public:
    Node(const SheetId parent=0)
        : parent(parent)
    {   /*  Nothing to do here  */  }

    const SheetId parent;
};

}   // namespace Graph
