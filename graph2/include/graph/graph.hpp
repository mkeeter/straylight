#pragma once

#include <map>

namespace Graph {

class Sheet;
class Cell;
class Instance;

class Graph
{
public:
    /*
     *  Installs data into the graph, returning a unique identifier
     */
    uint32_t install(Sheet* s);
    uint32_t install(Cell* c);
    uint32_t install(Instance* i);

protected:
    /*  This is the owned (canonical) location of data in the graph */
    std::map<uint32_t, std::unique_ptr<Sheet>> sheets;
    std::map<uint32_t, std::unique_ptr<Cell>> cells;
    std::map<uint32_t, std::unique_ptr<Instance>> instances;
};

}   // namespace Graph
