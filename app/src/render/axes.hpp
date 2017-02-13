#pragma once

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

namespace App {
namespace Render {

class Axes : public QOpenGLFunctions
{
public:
    Axes();
    void draw(QMatrix4x4 M);

protected:
    QOpenGLBuffer solid_vbo;
    QOpenGLVertexArrayObject solid_vao;

    QOpenGLBuffer wire_vbo;
    QOpenGLVertexArrayObject wire_vao;

    QOpenGLShaderProgram shader;
};

}   // namespace Render
}   // namespace App
