#include <QVector2D>

#include "render/drag.hpp"

namespace App {
namespace Render {

Drag::Drag(const Kernel::Tree& x, const Kernel::Tree& y, const Kernel::Tree& z)
    : cursor_pos{0,0,0}, cursor_ray{0,0,0}, d(0), d0(0)
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
    Kernel::Tree _d0 = Kernel::Tree::var(0);

    auto dx = x - (_cursor_pos[0] + _d*_cursor_ray[0]);
    auto dy = y - (_cursor_pos[1] + _d*_cursor_ray[1]);
    auto dz = z - (_cursor_pos[2] + _d*_cursor_ray[2]);

    err.reset(new Kernel::Evaluator(max(max(abs(dx), abs(dy)),
                    max(abs(_d - _d0), abs(dz)))));

    // Load variables IDs
    for (int i=0; i < 3; ++i)
    {
        cursor_pos[i] = _cursor_pos[i].var();
        cursor_ray[i] = _cursor_ray[i].var();
    }
    d = _d.var();
    d0 = _d0.var();
}

void Drag::update(const QMatrix4x4& M, const QVector2D& cursor,
                  const QVector3D& prev)
{
    QVector3D _cursor_pos = M * QVector3D(cursor.x(), cursor.y(), 0);
    QVector3D _cursor_ray = M * QVector3D(0, 0, -1) - M * QVector3D(0, 0, 0);

    // Set position and delta vector
    for (int i=0; i < 3; ++i)
    {
        err->setVar(cursor_pos[i], _cursor_pos[i]);
        err->setVar(cursor_ray[i], _cursor_ray[i]);
    }

    // Set base offset from prevoius position
    err->setVar(d0, QVector3D::dotProduct(prev - _cursor_pos, _cursor_ray));
}

}   // namespace Render
}   // namespace App
