#include "instance.hpp"
#include "sheet.hpp"

namespace Graph
{

Instance::Instance(Sheet* sheet)
    : sheet(sheet)
{
    // Construct inputs from default expressions
    for (auto c : sheet->inputs())
    {
        inputs[c.first] = c.second;
    }
}

}
