#pragma once

#include <string>

#include "graph/index.hpp"
#include "graph/cell.hpp"

class TreeSerializer
{
public:
    /*
     *  If this function call returns false, then we skip the sheet
     *  (used to minimize serializer effort)
     */
    virtual bool beginSheet(SheetIndex s)=0;
    virtual void endSheet()=0;

    /*
     *  Begin drawing an instance with the given name and index
     */
    virtual void instance(InstanceIndex i, const std::string& name)=0;

    /*
     *  Input and output cells are drawn within the containing instance
     */
    virtual void input(CellIndex c, const std::string& name,
                       const std::string& expr, bool valid,
                       const std::string& val)=0;
    virtual void output(CellIndex c, const std::string& name,
                        bool valid, const std::string& val)=0;

    /*
     *  Cells are drawn within the containing sheet
     */
    virtual void cell(CellIndex c, const std::string& name,
                      const std::string& expr, Cell::Type type,
                      bool valid, const std::string& val)=0;
};
