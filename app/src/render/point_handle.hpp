#pragma once

#include "render/handle.hpp"
#include "bind/bind_s7.h"

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

namespace App {
namespace Render {

class PointHandle : public Handle
{
public:
    PointHandle();

    void _draw(const QMatrix4x4& world, const QMatrix4x4& view,
               Handle::DrawMode mode) override;
    bool updateFrom(Graph::ValuePtr p) override;

protected:
    void initGL() override;
    void setVars(const QMatrix4x4& world, const QMatrix4x4& proj,
                 QOpenGLShaderProgram& shader);

    QVector3D center;

    // TODO: share these among all instances
    QOpenGLBuffer vbo;
    QOpenGLVertexArrayObject vao;
    QOpenGLShaderProgram shader_solid;
    QOpenGLShaderProgram shader_dotted;
    bool gl_ready=false;

    static const int segments=128;
};

}   // namespace Render
}   // namespace App
