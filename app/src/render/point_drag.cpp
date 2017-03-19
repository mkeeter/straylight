#include <QVector2D>

#include "app/render/point_drag.hpp"

namespace App {
namespace Render {

PointDrag::PointDrag()
    : cursor_pos{Kernel::Tree::var(),
                 Kernel::Tree::var(),
                 Kernel::Tree::var()},
      cursor_ray{Kernel::Tree::var(),
                 Kernel::Tree::var(),
                 Kernel::Tree::var()},
      d(Kernel::Tree::var())
{
    // Nothing to do here
}

bool PointDrag::updateFrom(App::Bridge::EscapedPointHandle* p)
{
    auto dx = p->xyz[0] - (cursor_pos[0] + d*cursor_ray[0]);
    auto dy = p->xyz[1] - (cursor_pos[1] + d*cursor_ray[1]);
    auto dz = p->xyz[2] - (cursor_pos[2] + d*cursor_ray[2]);

    // TODO: update evaluator variables instead of making a new one here
    // (if X/Y/Z trees are identical)
    vars = p->vars;

    // Load default values into variables to construct evaluator
    // (but don't leave the values in vars, because otherwise they'll
    //  mess with our attempts to change them later)
    auto vars_ = vars;
    for (int i=0; i < 3; ++i)
    {
        vars_.insert({cursor_pos[i].id(), 0});
        vars_.insert({cursor_ray[i].id(), 0});
    }
    vars_.insert({d.id(), 0});

    auto root = square(dx) + square(dy) + square(dz);
    err.reset(new Kernel::Evaluator(square(dx) + square(dy) + square(dz),
                                    vars_));

    return true;
}

Kernel::Solver::Solution PointDrag::dragTo(const QMatrix4x4& M,
                                           const QVector2D& cursor)
{
    auto world = toWorld(M, cursor, start);
    auto _cursor_pos = world.first;
    auto _cursor_ray = world.second;

    // Set position and delta vector
    std::set<Kernel::Tree::Id> masked;
    for (int i=0; i < 3; ++i)
    {
        err->setVar(cursor_pos[i].id(), _cursor_pos[i]);
        err->setVar(cursor_ray[i].id(), _cursor_ray[i]);
        masked.insert(cursor_pos[i].id());
        masked.insert(cursor_ray[i].id());
    }
    err->setVar(d.id(), 0);

    // TODO: this sets vars twice (once in Evaluator constructor, once here)
    auto sol = Kernel::Solver::findRoot(*err, vars, {0,0,0}, masked);
    sol.second.erase(d.id());

    return sol.second;
}

}   // namespace Render
}   // namespace App
