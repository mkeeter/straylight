#include "app/bridge/escaped.hpp"
#include "app/render/point_handle.hpp"

namespace App {
namespace Bridge {

EscapedShape::EscapedShape(Kernel::Tree tree)
    : eval(new Kernel::Evaluator(tree))
{
    // Nothing to do here
}

EscapedPointHandle::EscapedPointHandle(
        QVector3D p, std::unique_ptr<App::Render::PointDrag>& d)
    : pos(p), drag(d.release())
{
    // Nothing to do here
}

int EscapedPointHandle::tag() const
{
    return App::Render::PointHandle::_tag;
}

}   // namespace Bridge
}   // namespace App
