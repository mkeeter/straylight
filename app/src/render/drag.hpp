#pragma once

#include "kernel/eval/evaluator.hpp"
#include "kernel/tree/cache.hpp"

namespace App {
namespace Render {

class Drag
{
public:
    /*
     *  Constructs a dragger object for a single point
     *
     *  This must be called in the same thread as the Kernel objects
     */
    Drag(const Kernel::Tree& x, const Kernel::Tree& y, const Kernel::Tree& z);

protected:
    std::unique_ptr<Kernel::Evaluator> err;

    /*
     *  The mouse cursor line is represented by
     *  v + d*p
     *  where v and p are 3-vectors and d is a scalar
     *
     *  d0 is the value of d at the start position
     *  We want to minimize abs(d - d0), otherwise we could drift
     *  arbitrarily along the cursor pointing line.
     *
     *  The solver will find a solution in terms of the base variables
     *  and d; we lock v, p, and d0.
     */
    Kernel::Cache::VarId v[3];
    Kernel::Cache::VarId p[3];
    Kernel::Cache::VarId d;
    Kernel::Cache::VarId d0;
};

}   // namespace Render
}   // namespace App
