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
};

}   // namespace Render
}   // namespace App
