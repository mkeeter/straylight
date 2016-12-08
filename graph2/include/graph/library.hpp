#pragma once

#include <boost/bimap.hpp>

#include "index.hpp"
#include "keys.hpp"
#include "sheet.hpp"

class Library
{
public:
    /*
     *  By default, construct sheet 0
     *  Sheet 0 has no parent and no name
     */
    Library();

    /*
     *  Check to see whether we can insert the given sheet
     */
    bool canInsert(NameKey key) const;

    /*
     *  Inserts the given sheet, returning its index
     */
    SheetIndex insert(std::string name, SheetIndex parent={0});

    /*
     *  Renames a particular sheet
     */
    void rename(SheetIndex sheet, std::string new_name);

    /*
     *  Erases the given sheet
     */
    void erase(SheetIndex s);

    /*
     *  Get a sheet by index
     */
    const Sheet& operator[](SheetIndex i) const
        { return sheets.at(i); }

    /*
     *  Get a sheet's name
     */
    const std::string& nameOf(SheetIndex i) const
        { return names.right.at(i).second; }

    /*
     *  Iterate over sheets in alphabetical order for a given parent
     */
    std::list<SheetIndex> iterSheets(SheetIndex parent) const;

protected:
    /*  Master storage for all sheets.  Indices are unique.  */
    std::map<SheetIndex, Sheet> sheets;

    /*  This maps name + parent pairs to sheet indices */
    boost::bimap<NameKey, SheetIndex> names;
};
