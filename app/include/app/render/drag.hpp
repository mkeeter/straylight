#pragma once

#include <QMatrix4x4>

#include "kernel/solve/solver.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace App {
namespace Render {

class Drag
{
public:
    virtual ~Drag() { /* Nothing to do here */ }

    /*
     *  Drags to a particular position
     */
    virtual Kernel::Solver::Solution dragTo(const QMatrix4x4& M,
                                            const QVector2D& cursor)=0;

    /*
     *  Returns a pair of vectors [pos, ray], where the mouse line can
     *  be defined as pos + d*ray in world coordinates
     *
     *  M is the world-to-render matrix
     *  cursor is the cursor position in OpenGL coordinates [+/-1, +/-1]
     *  start is the starting position in world coordinates;
     *    the returned pos is chosen to be as close to start as possible
     */
    static QPair<QVector3D, QVector3D> toWorld(const QMatrix4x4& M,
                                               const QVector2D& cursor,
                                               const QVector3D start);
};

}   // namespace Render
}   // namespace App
