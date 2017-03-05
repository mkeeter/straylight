#include "app/render/handle.hpp"

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

void Handle::glUniformColor3f(QOpenGLShaderProgram& shader, const QString& var,
                             const QColor& color)
{
    glUniform3f(shader.uniformLocation(var),
                color.red()/255.0f, color.green()/255.0f, color.blue()/255.0f);
}

void Handle::glUniformColor3f(QOpenGLShaderProgram& shader, const QString& var,
                             const QRgb color)
{
    glUniformColor3f(shader, var, QColor::fromRgb(color));
}


}   // namespace Render
}   // namespace App
