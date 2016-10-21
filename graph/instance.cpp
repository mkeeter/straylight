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

    // Record oneself in the sheet's list of instances
    if (sheet->parent)
    {
        for (auto e : sheet->parent->envs)
        {
            e.push_back(this);
            sheet->envs.insert(e);
        }
    }
    else
    {
        sheet->envs.insert({this});
    }
}

Instance::~Instance()
{
    if (sheet->parent)
    {
        for (auto e : sheet->parent->envs)
        {
            e.push_back(this);
            sheet->envs.erase(e);
        }
    }
    else
    {
        sheet->envs.erase({this});
    }
}

}
