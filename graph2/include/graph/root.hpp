#pragma once

#include <map>

#include "graph/id.hpp"

namespace Graph {

struct Sheet;
struct Cell;
struct Instance;

struct Root
{
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
