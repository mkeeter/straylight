#pragma once

#include <string>
#include "s7.h"

class Datum
{
public:
    Datum(std::string expr, s7_scheme* s7);
    void update();

    /*  Expression to evaluation */
    char expr[4096] = {0};

    /*  Count the number of newlines in the expression
     *  (used for layout) */
    int newlines() const;

    /*  Resulting value and value string */
    s7_pointer val;
    char* val_str;

    /*  Pointer back to interpreter  */
    s7_scheme* const s7;
};
