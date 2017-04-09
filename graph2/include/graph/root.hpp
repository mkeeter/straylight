#pragma once

#include <map>

#include "graph/id.hpp"

namespace Graph {

struct Sheet;
struct Cell;
struct Instance;

struct Root
{
    /*
     *  Insertion functions both store a node and add it to the parent sheet
     */
    CellId insert(const std::string& name, Cell* c);
    InstanceId insert(const std::string& name, Instance* i);
    SheetId insert(const std::string& name, Sheet* s);

    /*  This is the owned (canonical) location of data in the graph */
    IdMap<SheetId, Sheet, 1, 2> sheets;
    IdMap<CellId, Cell, 3, 2> cells;
    IdMap<InstanceId, Instance, 2, 2> instances;
};

}   // namespace Graph

// For unique_ptr destructors
#include "graph/sheet.hpp"
#include "graph/cell.hpp"
#include "graph/instance.hpp"
