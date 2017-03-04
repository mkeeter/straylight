
#include "app/render/shape_handle.hpp"
#include "app/bridge/escaped.hpp"

namespace App {
namespace Render {

void ShapeHandle::_draw(const QMatrix4x4& world, const QMatrix4x4& proj,
                        Picker::DrawMode mode, QRgb color)
{
    // TODO
}

bool ShapeHandle::updateFrom(App::Bridge::EscapedHandle* h)
{
    auto s = dynamic_cast<Bridge::EscapedShape*>(h);
    assert(s);

    eval.reset(s->eval);
    return false;
}

void ShapeHandle::updateTexture(const Renderer::Result imgs)
{
    // TODO
}

Drag* ShapeHandle::getDrag(const QMatrix4x4& M, const QVector2D& cursor)
{
    // TODO
}

void ShapeHandle::initGL()
{
    // TODO
}

}   // namespace Render
}   // namespace App
