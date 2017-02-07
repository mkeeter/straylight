#pragma once

#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QQuickWindow>

#include "render/renderer.hpp"

namespace App {
namespace Render {

class Blitter : public QObject, public QOpenGLFunctions
{
    Q_OBJECT
public:
    Blitter();
    void draw(QMatrix4x4 M);

public slots:
    /*  Both addQuad and forget are async, scheduling work to take place
     *  in the render thread. */
    void addQuad(Renderer* R, const Renderer::Result imgs,
                 const QMatrix4x4& t);
    void forget(Renderer*);

signals:
    /*
     *  Used when a quad is added; instructs parent canvas to redraw
     */
    void changed();

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

    /*
     *  Looks through the QML hierarchy to find a QQuickWindow
     *  asserts(false) if none is present
     */
    QQuickWindow* getWindow() const;

    std::map<Renderer*, Quad*> quads;

    QOpenGLBuffer quad_vbo;
    QOpenGLVertexArrayObject quad_vao;
    QOpenGLShaderProgram shader;

    ////////////////////////////////////////////////////////////////////////////
    // Dummy classes to be used by scheduleRenderJob
    // (used to pass OpenGL texture creation / deletion into render thread)
    class QuadAdd : public QRunnable
    {
    public:
        QuadAdd(Blitter* parent, Renderer* R, const QMatrix4x4& mat,
                const Kernel::DepthImage* d, const Kernel::NormalImage* n);
        void run() override;

    protected:
        Blitter* const parent;
        Renderer* const R;

        const QMatrix4x4 mat;
        const Kernel::DepthImage* depth;
        const Kernel::NormalImage* norm;
    };

    class QuadForget : public QRunnable
    {
    public:
        QuadForget(Blitter* parent, Renderer* R);
        void run() override;

    protected:
        Blitter* const parent;
        Renderer* const R;
    };
};

}   // namespace Render
}   // namespace App
