#pragma once

#include <string>
#include "s7.h"

class Datum
{
public:
    Datum(const std::string& expr, s7_scheme* s7);
    void setExpr(const std::string& expr);
    void update();

    /*  Expression to evaluation */
    std::string expr;

    /*  Count the number of newlines in the expression
     *  (used for layout) */
    int newlines() const;

    /*  Check to see whether we can rename to the given name
     */
    bool canRenameTo(const std::string& n);

    /*  Resulting value and value string */
    s7_pointer val;
    char* val_str;

    /*  Pointer back to interpreter  */
    s7_scheme* const s7;
};
