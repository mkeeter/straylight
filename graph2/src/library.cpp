#include "graph/library.hpp"

Library::Library()
{
    storage.insert({{0}, {}});
}

SheetIndex Library::insert(const std::string& name, SheetIndex parent)
{
    return KeyNameStore::insert(name, {}, parent);
}
