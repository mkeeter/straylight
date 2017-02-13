#include "render/handle.hpp"

namespace App {
namespace Render {

void Handle::draw(const QMatrix4x4& world, const QMatrix4x4& proj,
                  Handle::DrawMode mode)
{
    if (!gl_ready)
    {
        initializeOpenGLFunctions();
        initGL();
        gl_ready = true;
    }
    _draw(world, proj, mode);
}

}   // namespace Render
}   // namespace App
