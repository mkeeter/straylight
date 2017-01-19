#include "graph/library.hpp"

namespace Graph {

Library::Library()
{
    storage.insert({{0}, {}});
}

SheetIndex Library::insert(SheetIndex parent, const std::string& name)
{
    return KeyNameStore::insert(parent, name, {});
}

void Library::insert(SheetIndex parent, SheetIndex sheet,
                     const std::string& name)
{
    return KeyNameStore::insert(parent, sheet, name, {});
}

}   // namespace Graph
