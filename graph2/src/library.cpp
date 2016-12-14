#include "graph/library.hpp"

Library::Library()
{
    storage.insert({{0}, {}});
}

SheetIndex Library::insert(SheetIndex parent, const std::string& name)
{
    return KeyNameStore::insert(parent, name, {});
}
