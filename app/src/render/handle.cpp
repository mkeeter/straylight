#include "render/handle.hpp"

namespace App {
namespace Render {

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

}   // namespace Render
}   // namespace App
