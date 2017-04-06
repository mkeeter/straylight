#pragma once

#include <map>

#include "graph/id.hpp"

namespace Graph {

struct Sheet;
struct Cell;
struct Instance;

class Root
{
public:
    /*
     *  Installs data into the graph, returning a unique identifier
     */
    SheetId install(Sheet* s);
    CellId install(Cell* c);
    InstanceId install(Instance* i);

    /*  This is the owned (canonical) location of data in the graph */
    std::map<SheetId, std::unique_ptr<Sheet>> sheets;
    std::map<CellId, std::unique_ptr<Cell>> cells;
    std::map<InstanceId, std::unique_ptr<Instance>> instances;
};

}   // namespace Graph
