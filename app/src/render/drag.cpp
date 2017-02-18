#include <QVector2D>

#include "render/drag.hpp"

namespace App {
namespace Render {

Drag::Drag(const Kernel::Tree& x, const Kernel::Tree& y, const Kernel::Tree& z)
    : cursor_pos{0,0,0}, cursor_ray{0,0,0}, d(0)
{
    Kernel::Tree _cursor_pos[3] = {
        Kernel::Tree::var(0),
        Kernel::Tree::var(0),
        Kernel::Tree::var(0)
    };

    Kernel::Tree _cursor_ray[3] = {
        Kernel::Tree::var(0),
        Kernel::Tree::var(0),
        Kernel::Tree::var(0)
    };

    Kernel::Tree _d = Kernel::Tree::var(0);

    auto dx = x - (_cursor_pos[0] + _d*_cursor_ray[0]);
    auto dy = y - (_cursor_pos[1] + _d*_cursor_ray[1]);
    auto dz = z - (_cursor_pos[2] + _d*_cursor_ray[2]);

    err.reset(new Kernel::Evaluator(
                square(dx) + square(dy) + square(dz)+ square(_d)));

    // Load variables IDs
    for (int i=0; i < 3; ++i)
    {
        cursor_pos[i] = _cursor_pos[i].var();
        cursor_ray[i] = _cursor_ray[i].var();
    }
    d = _d.var();
}

Kernel::Solver::Solution Drag::dragTo(const QMatrix4x4& M,
                                      const QVector2D& cursor)
{
    QVector3D _cursor_ray = M * QVector3D(0, 0, -1) - M * QVector3D(0, 0, 0);

    // Position _cursor_pos to minimize travel along the ray from start point
    // TODO: clean up this vector math
    QVector3D base_pos = M * QVector3D(cursor.x(), cursor.y(), 0);
    QVector3D _cursor_pos = base_pos + _cursor_ray *
        QVector3D::dotProduct(start - base_pos, _cursor_ray);

    // Set position and delta vector
    std::set<Kernel::Cache::VarId> masked;
    for (int i=0; i < 3; ++i)
    {
        err->setVar(cursor_pos[i], _cursor_pos[i]);
        err->setVar(cursor_ray[i], _cursor_ray[i]);
        masked.insert(cursor_pos[i]);
        masked.insert(cursor_ray[i]);
    }
    err->setVar(d, 0);

    auto sol = Kernel::Solver::findRoot(*err, {0,0,0}, masked);
    sol.second.erase(d);
    return sol.second;
}

}   // namespace Render
}   // namespace App
