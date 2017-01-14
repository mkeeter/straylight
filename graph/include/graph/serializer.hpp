#pragma once

#include <string>

#include "graph/index.hpp"
#include "graph/cell.hpp"

namespace Graph {

class TreeSerializer
{
public:
    /*
     *  Describes a cell within the current sheet
     */
    virtual void cell(CellIndex c, const std::string& name,
                      const std::string& expr, Cell::Type type,
                      bool valid, const std::string& val,
                      const ValuePtr ptr)=0;

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
    virtual bool push(InstanceIndex i, const std::string& instance_name,
                      const std::string& sheet_name)=0;

    /*
     *  Called to note that the current environment contains the given sheet
     *
     *  The sheet is marked as editable if it lives within the environment
     *  (rather than a parent environment), and is insertable if inserting an
     *  instance will not create a loop
     */
    virtual void sheet(SheetIndex s, const std::string& sheet_name,
                       bool editable, bool insertable)=0;

    virtual void pop()=0;
};

class FlatSerializer
{
public:
    /*
     *  Describes a cell within the current sheet
     */
    virtual void cell(CellIndex c, const std::string& name,
                      const std::string& expr)=0;

    /*
     *  Begins an instance within the current sheet
     *
     *  input calls after this point refer to this instance
     *  (until reset by another call to instance)
     */
    virtual void instance(InstanceIndex i, const std::string& name,
                          SheetIndex s)=0;

    /*
     *  Stores an input expression
     */
    virtual void input(CellIndex c, const std::string& expr)=0;

    /*
     *  Pushes and pops into a sheet from the library
     */
    virtual void push(SheetIndex i, const std::string& sheet_name)=0;
    virtual void pop()=0;
};

}   // namespace Graph
