#pragma once

#include <QMatrix4x4>
#include <QVector3D>
#include <QPoint>

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

    /*
     *  Loads v, p, and d0
     *  given the given transform matrix, cursor position, and previous point
     *
     *  Cursor position is relative to OpenGL window space (±1 on both axes)
     */
    void update(const QMatrix4x4& M, const QVector2D& cursor,
                const QVector3D& prev);

protected:
    std::unique_ptr<Kernel::Evaluator> err;

    /*
     *  The mouse cursor line is represented by
     *  cursor_pos + d*cursor_ray
     *  where pos and ray are 3-vectors and d is a scalar
     *
     *  d0 is the value of d at the start position
     *  We want to minimize abs(d - d0), otherwise we could drift
     *  arbitrarily along the cursor pointing line.
     *
     *  The solver will find a solution in terms of the base variables
     *  and d; we lock pos, ray, and d0.
     */
    Kernel::Cache::VarId cursor_pos[3];
    Kernel::Cache::VarId cursor_ray[3];
    Kernel::Cache::VarId d;
    Kernel::Cache::VarId d0;
};

}   // namespace Render
}   // namespace App
