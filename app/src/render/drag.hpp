#pragma once

#include <QMatrix4x4>
#include <QVector3D>
#include <QPoint>

#include "kernel/eval/evaluator.hpp"
#include "kernel/tree/cache.hpp"
#include "kernel/solve/solver.hpp"

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

    /*
     *  Loads a starting position for a drag operation
     */
    void startDrag(const QVector3D& p) { start = p; }

    /*
     *  Drags to a particular position
     */
    Kernel::Solver::Solution dragTo(const QMatrix4x4& M,
                                    const QVector2D& cursor);

protected:
    std::unique_ptr<Kernel::Evaluator> err;

    /*
     *  The mouse cursor line is represented by
     *  cursor_pos + d*cursor_ray
     *  where pos and ray are 3-vectors and d is a scalar
     *
     *  d should be set to 0 at the point closest to the previous point,
     *  to minimize drift into the screen.
     *
     *  We want to minimize abs(d), otherwise we could drift
     *  arbitrarily along the cursor pointing line.
     *
     *  The solver will find a solution in terms of the base variables
     *  and d; we lock pos and ray
     */
    Kernel::Cache::VarId cursor_pos[3];
    Kernel::Cache::VarId cursor_ray[3];
    Kernel::Cache::VarId d;

    /*  We store the starting position for a drag here
     *
     *  This lets us minimize changes in the z plane (which can't be
     *  controlled by the user)     */
    QVector3D start;
};

}   // namespace Render
}   // namespace App
