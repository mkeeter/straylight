#include "graph/root.hpp"

namespace Graph {

SheetId Root::insert(const std::string& name, Sheet* s)
{
    auto i = sheets.insert(s);
    sheets.at(s->parent)->sheets.insert({name, i});
    return i;
}

CellId Root::insert(const std::string& name, Cell* s)
{
    auto i = cells.insert(s);
    sheets.at(s->parent)->cells.insert({name, i});
    return i;
}

InstanceId Root::insert(const std::string& name, Instance* s)
{
    auto i = instances.insert(s);
    sheets.at(s->parent)->instances.insert({name, i});
    return i;
}


}   // namespace Graph
