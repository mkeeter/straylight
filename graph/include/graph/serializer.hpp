#pragma once

#include <string>

#include "graph/index.hpp"
#include "graph/cell.hpp"

class TreeSerializer
{
public:
    /*
     *  Describes a cell within the current sheet
     */
    virtual void cell(CellIndex c, const std::string& name,
                      const std::string& expr, Cell::Type type,
                      bool valid, const std::string& val)=0;

    /*
     *  Begin drawing an instance with the given name and index
     *
     *  The instance will describe a set of inputs and outputs
     *  (calling the functions below), then a push / pop pair
     *  (with optional more things in between)
     */
    virtual void instance(InstanceIndex i, const std::string& name,
                          const std::string& sheet)=0;

    /*
     *  Input and output cells are drawn within the containing instance
     */
    virtual void input(CellIndex c, const std::string& name,
                       const std::string& expr, bool valid,
                       const std::string& val)=0;
    virtual void output(CellIndex c, const std::string& name,
                        bool valid, const std::string& val)=0;

    /*
     *  Requests a push into the most recent instance
     *
     *  If push returns false, then pop will be called immediately
     *  Otherwise, we'll describe all the stuff we see in the instance
     *  (recursively as needed)
     */
    virtual bool push()=0;
    virtual void pop()=0;
};
