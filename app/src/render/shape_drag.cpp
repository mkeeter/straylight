#include "app/render/shape_drag.hpp"

namespace App {
namespace Render {

void ShapeDrag::startDrag(const QVector3D& p)
{
    start = p;
    shape->set(p.x(), p.y(), p.z(), 0);
    auto ds = shape->derivs(1);
    norm = QVector3D(ds.dx[0], ds.dy[0], ds.dz[0]);

    // TODO:
    // Specialize shape here
}

bool ShapeDrag::updateFrom(App::Bridge::EscapedShape* s)
{
    // TODO: only update shape if evaluator is different
    shape.reset(new Kernel::Evaluator(s->tree, s->vars));
    vars = s->vars;

    return true;
}

Kernel::Solver::Solution ShapeDrag::dragTo(const QMatrix4x4& M,
                                           const QVector2D& cursor)
{
    const auto world = toWorld(M, cursor, start);
    const auto cursor_pos = world.first;
    const auto cursor_ray = world.second;

    // Solve for the point on the normal ray that is closest to the cursor ray
    // https://en.wikipedia.org/wiki/Skew_lines#Distance_between_two_skew_lines
    const auto n = QVector3D::crossProduct(norm, cursor_ray);
    const auto n2 = QVector3D::crossProduct(cursor_ray, n);
    auto nearest = start + QVector3D::dotProduct(cursor_pos - start, n2) /
        QVector3D::dotProduct(norm, n2) * norm;

    // TODO: this sets vars twice (once in Evaluator constructor, once here)
    auto sol = Kernel::Solver::findRoot(
            *shape, vars, {nearest.x(), nearest.y(), nearest.z()});

    return sol.second;
}

}   // namespace Render
}   // namespace App
