#include "app/bridge/escaped.hpp"
#include "app/render/point_handle.hpp"

namespace App {
namespace Bridge {

EscapedShape::EscapedShape(Kernel::Tree tree)
    : eval(new Kernel::Evaluator(tree))
{
    // Nothing to do here
}

EscapedPointHandle::EscapedPointHandle(Kernel::Tree xyz[3])
    : pos(xyz[0].value(), xyz[1].value(), xyz[2].value()),
      xyz{Kernel::Evaluator(xyz[0]),
          Kernel::Evaluator(xyz[1]),
          Kernel::Evaluator(xyz[2])}
{
    // Nothing to do here
}

int EscapedPointHandle::tag() const
{
    return App::Render::PointHandle::_tag;
}

}   // namespace Bridge
}   // namespace App
