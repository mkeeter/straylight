#include <cassert>
#include "datum.hpp"

Datum::Datum(const std::string& _expr, s7_scheme* s7)
    : s7(s7)
{
    assert(_expr.size() < sizeof(expr));
    std::copy(_expr.begin(), _expr.end(), expr);
    update();
}

void Datum::update()
{
    val = s7_eval_c_string(s7, expr);
    val_str = s7_object_to_c_string(s7, val);
}

bool Datum::canRenameTo(const std::string& n)
{
    return std::count(n.begin(), n.end(), ' ') == 0 &&
           n.size() > 0 && !isdigit(n[0]);
}

int Datum::newlines() const
{
    int i = 0;
    for (auto& c : expr)
    {
        if (!c)
            return i;
        else if (c == '\n')
            i++;
    }
    return i;
}
