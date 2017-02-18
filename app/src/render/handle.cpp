#include "render/handle.hpp"

namespace App {
namespace Render {

Handle::Handle(std::unique_ptr<Drag>& drag)
    : drag(drag.release())
{
    // Nothing to do here
}

void Handle::draw(const QMatrix4x4& world, const QMatrix4x4& proj,
                  Picker::DrawMode mode, QRgb color)
{
    if (!gl_ready)
    {
        initializeOpenGLFunctions();
        initGL();
        gl_ready = true;
    }
    _draw(world, proj, mode, color);
}

Drag* Handle::getDrag(const QMatrix4x4& M, const QVector2D& cursor)
{
    drag->startDrag(pos(M, cursor));
    return drag.get();
}

}   // namespace Render
}   // namespace App
