#include "render/drag.hpp"

namespace App {
namespace Render {

Drag::Drag(const Kernel::Tree& x, const Kernel::Tree& y, const Kernel::Tree& z)
    : v{0,0,0}, p{0,0,0}, d(0), d0(0)
{
    Kernel::Tree _v[3] = {
        Kernel::Tree::var(0),
        Kernel::Tree::var(0),
        Kernel::Tree::var(0)
    };

    Kernel::Tree _p[3] = {
        Kernel::Tree::var(0),
        Kernel::Tree::var(0),
        Kernel::Tree::var(0)
    };

    Kernel::Tree _d = Kernel::Tree::var(0);
    Kernel::Tree _d0 = Kernel::Tree::var(0);

    auto dx = x - (_v[0] + _d*_p[0]);
    auto dy = y - (_v[1] + _d*_p[1]);
    auto dz = z - (_v[2] + _d*_p[2]);

    err.reset(new Kernel::Evaluator(max(max(abs(dx), abs(dy)),
                    max(abs(_d - _d0), abs(dz)))));

    for (int i=0; i < 3; ++i)
    {
        v[i] = _v[i].var();
        p[i] = _p[i].var();
    }
    d = _d.var();
    d0 = _d0.var();
}

}   // namespace Render
}   // namespace App
