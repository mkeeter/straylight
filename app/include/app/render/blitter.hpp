#pragma once

#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QQuickWindow>

#include "app/render/renderer.hpp"

namespace App {
namespace Render {

class Blitter : public QObject, public QOpenGLFunctions
{
    Q_OBJECT
public:
    Blitter();
    void draw(QMatrix4x4 M);

    /*
     *  Adds a new quad to the renderer
     *
     *  This must be called from the graphics thread, as it performs
     *  OpenGL operations to generate textures
     */
    void addQuad(Renderer* R, const Renderer::Result imgs);

    /*
     *  Removes any quads that are not represented in rs
     *
     *  This must be called from the graphics thread
     */
    void prune(const QSet<Renderer*>& rs);

protected:
    /*
     *  Struct that represents a textured quad in 3D space
     */
    struct Quad {
        Quad(const QMatrix4x4& mat, const Kernel::DepthImage* d,
             const Kernel::NormalImage* n);
        QMatrix4x4 mat;
        QOpenGLTexture depth;
        QOpenGLTexture norm;
    };

    std::map<Renderer*, Quad*> quads;

    /*  OpenGL shader objects  */
    QOpenGLBuffer quad_vbo;
    QOpenGLVertexArrayObject quad_vao;
    QOpenGLShaderProgram shader;
};

}   // namespace Render
}   // namespace App
