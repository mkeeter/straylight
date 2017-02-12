#include "render/handle.hpp"

namespace App {
namespace Render {

void Handle::draw(const QMatrix4x4& m, bool selected)
{
    if (!gl_ready)
    {
        initializeOpenGLFunctions();
        initGL();
        gl_ready = true;
    }
    _draw(m, selected);
}

}   // namespace Render
}   // namespace App
