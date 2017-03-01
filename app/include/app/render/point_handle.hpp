#pragma once

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

#include "app/render/handle.hpp"
#include "app/bind/bind_s7.hpp"

namespace App {
namespace Render {

class PointHandle : public Handle
{
public:
    PointHandle(std::unique_ptr<Drag>& drag);

    void _draw(const QMatrix4x4& world, const QMatrix4x4& view,
               Picker::DrawMode mode, QRgb color=0) override;
    bool updateFrom(Graph::ValuePtr p) override;

protected:
    void initGL() override;
    QVector3D pos(const QMatrix4x4& M, const QVector2D& cursor) const override;

    void setVars(const QMatrix4x4& world, const QMatrix4x4& proj,
                 QOpenGLShaderProgram& shader, Picker::DrawMode mode,
                 QRgb color);

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
