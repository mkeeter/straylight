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
    PointHandle(const App::Bind::point_handle_t* p);

    void _draw(const QMatrix4x4& m, bool selected) override;
    void updateFrom(Graph::ValuePtr p) override;

protected:
    void initGL() override;

    bool gl_ready;
    QOpenGLBuffer vbo;
    QOpenGLVertexArrayObject vao;
    QOpenGLShaderProgram shader;

    static const int segments=16;
};

}   // namespace Render
}   // namespace App
