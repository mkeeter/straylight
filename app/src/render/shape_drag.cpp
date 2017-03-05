#include "app/render/shape_drag.hpp"

namespace App {
namespace Render {

void ShapeDrag::startDrag(const QVector3D& p)
{
    // TODO
}

bool ShapeDrag::updateFrom(App::Bridge::EscapedShape* s)
{
    shape.reset(s->eval);

    // TODO: only update shape if evaluator is different
    return true;
}

Kernel::Solver::Solution ShapeDrag::dragTo(const QMatrix4x4& M,
                                           const QVector2D& cursor)
{
    // TODO
    return Kernel::Solver::Solution();
}

}   // namespace Render
}   // namespace App
