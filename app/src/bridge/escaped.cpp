#include "app/bridge/escaped.hpp"

#include "app/render/point_handle.hpp"
#include "app/render/shape_handle.hpp"

namespace App {
namespace Bridge {

EscapedShape::EscapedShape(Kernel::Tree tree,
                 const std::map<Kernel::Tree::Id, float>& vars)
    : EscapedHandle(vars), tree(tree)
{
    // Nothing to do here
}

int EscapedShape::tag() const
{
    return App::Render::ShapeHandle::_tag;
}

////////////////////////////////////////////////////////////////////////////////

EscapedPointHandle::EscapedPointHandle(Kernel::Tree xyz[3],
                 const std::map<Kernel::Tree::Id, float>& vars)
    : EscapedHandle(vars), xyz { xyz[0], xyz[1], xyz[2] }
{
    // Figure out position from tree and variable values
    pos.setX(Kernel::Evaluator(xyz[0], vars).values(1)[0]);
    pos.setY(Kernel::Evaluator(xyz[1], vars).values(1)[0]);
    pos.setZ(Kernel::Evaluator(xyz[2], vars).values(1)[0]);
}

int EscapedPointHandle::tag() const
{
    return App::Render::PointHandle::_tag;
}

////////////////////////////////////////////////////////////////////////////////

}   // namespace Bridge
}   // namespace App
