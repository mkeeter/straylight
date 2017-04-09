#include "graph/sheet.hpp"

namespace Graph {

Sheet::Sheet()
    : Sheet(0)
{
    // Nothing to do here
}

Sheet::Sheet(SheetId parent)
    : Node(parent)
{
    // Nothing to do here
}

}   // using namespace Graph
