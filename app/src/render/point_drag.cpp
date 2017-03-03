#include <QVector2D>

#include "app/render/point_drag.hpp"

namespace App {
namespace Render {

PointDrag::PointDrag()
    : cursor_pos{Kernel::Tree::var(0),
                 Kernel::Tree::var(0),
                 Kernel::Tree::var(0)},
      cursor_ray{Kernel::Tree::var(0),
                 Kernel::Tree::var(0),
                 Kernel::Tree::var(0)},
      d(Kernel::Tree::var(0))
{
    // Nothing to do here
}

bool PointDrag::updateFrom(App::Bridge::EscapedPointHandle* p)
{
    auto x = p->xyz[0].toTree(vars);
    auto y = p->xyz[0].toTree(vars);
    auto z = p->xyz[0].toTree(vars);

    auto dx = x - (cursor_pos[0] + d*cursor_ray[0]);
    auto dy = y - (cursor_pos[1] + d*cursor_ray[1]);
    auto dz = z - (cursor_pos[2] + d*cursor_ray[2]);

    err.reset(new Kernel::Evaluator(
                square(dx) + square(dy) + square(dz)));

    return true;
}

Kernel::Solver::Solution PointDrag::dragTo(const QMatrix4x4& M,
                                           const QVector2D& cursor)
{
    const QVector3D offset = M * QVector3D(0, 0, 0);
    const QVector3D _cursor_ray = M * QVector3D(0, 0, -1) - offset;

    // Position _cursor_pos to minimize travel along the ray from start point
    // TODO: clean up this vector math
    const QVector3D base_pos = M * QVector3D(cursor.x(), cursor.y(), 0);
    const QVector3D _cursor_pos = base_pos + _cursor_ray *
        QVector3D::dotProduct(start - base_pos, _cursor_ray);

    // Set position and delta vector
    std::set<Kernel::Cache::VarId> masked;
    for (int i=0; i < 3; ++i)
    {
        err->setVar(cursor_pos[i].var(), _cursor_pos[i]);
        err->setVar(cursor_ray[i].var(), _cursor_ray[i]);
        masked.insert(cursor_pos[i].var());
        masked.insert(cursor_ray[i].var());
    }
    err->setVar(d.var(), 0);

    auto sol = Kernel::Solver::findRoot(*err, {0,0,0}, masked);
    sol.second.erase(d.var());

    // Remap variables from our local thread to the interpreter thread
    {   // (so that the GraphModel can match them up with CellKeys)
        std::set<Kernel::Cache::VarId> local_vars;
        for (auto v : sol.second)
        {
            local_vars.insert(v.first);
        }
        for (auto v : local_vars)
        {
            auto f = sol.second.at(v);
            sol.second.erase(v);
            sol.second.insert({vars.right.at(v), f});
        }
    }

    return sol.second;
}

}   // namespace Render
}   // namespace App
