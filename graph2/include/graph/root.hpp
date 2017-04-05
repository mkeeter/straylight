#pragma once

#include <map>

#include "graph/id.hpp"

namespace Graph {

class Sheet;
class Cell;
class Instance;

class Root
{
public:
    /*
     *  Installs data into the graph, returning a unique identifier
     */
    uint32_t install(Sheet* s);
    uint32_t install(Cell* c);
    uint32_t install(Instance* i);

    /*  This is the owned (canonical) location of data in the graph */
    std::map<SheetId, std::unique_ptr<Sheet>> sheets;
    std::map<CellId, std::unique_ptr<Cell>> cells;
    std::map<InstanceId, std::unique_ptr<Instance>> instances;
};

}   // namespace Graph
