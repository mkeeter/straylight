#include <QGuiApplication>
#include <QThread>

#include "app/render/blitter.hpp"

namespace App {
namespace Render {

Blitter::Blitter()
{
    qRegisterMetaType<Renderer::Result>("Result");

    initializeOpenGLFunctions();

    shader.addShaderFromSourceFile(
            QOpenGLShader::Vertex, ":/gl/depthquad.vert");
    shader.addShaderFromSourceFile(
            QOpenGLShader::Fragment, ":/gl/depthquad.frag");
    shader.link();

    // Data is arranged  x   y
    GLfloat data[] = {  -1, -1,
                         1, -1,
                         1,  1,
                        -1,  1, };
    quad_vbo.create();
    quad_vbo.bind();
    quad_vbo.allocate(data, sizeof(data));

    quad_vao.create();
    quad_vao.bind();

    glVertexAttribPointer(
            0, 2, GL_FLOAT, GL_FALSE,
            2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    quad_vbo.release();
    quad_vao.release();
}

void Blitter::addQuad(Renderer* R, const Renderer::Result imgs,
                      const QMatrix4x4& mat)
{
    getWindow()->scheduleRenderJob(
            new QuadAdd(this, R, mat, imgs.depth, imgs.norm),
            QQuickWindow::NoStage);
}

void Blitter::forget(Renderer* R)
{
    getWindow()->scheduleRenderJob(
            new QuadForget(this, R),
            QQuickWindow::NoStage);
}

////////////////////////////////////////////////////////////////////////////////

void Blitter::draw(QMatrix4x4 M)
{
    shader.bind();
    glUniformMatrix4fv(shader.uniformLocation("m"), 1, GL_FALSE, M.data());
    auto mquad = shader.uniformLocation("m_quad");

    glUniform1i(shader.uniformLocation("depth"), 0);
    glUniform1i(shader.uniformLocation("norm"), 1);

    quad_vao.bind();
    for (auto& q : quads)
    {
        q.second->depth.bind(0);
        q.second->norm.bind(1);
        glUniformMatrix4fv(mquad, 1, GL_FALSE, q.second->mat.data());

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

    quad_vao.release();
    shader.release();

    // Reset active texture to avoid corrupting QML state
    glActiveTexture(GL_TEXTURE0);
}

QQuickWindow* Blitter::getWindow() const
{
    auto app = dynamic_cast<QGuiApplication*>(QCoreApplication::instance());
    assert(app);
    for (auto w : app->allWindows())
    {
        if (auto q = dynamic_cast<QQuickWindow*>(w))
        {
            return q;
        }
    }

    assert(false);
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////

Blitter::Quad::Quad(const QMatrix4x4& mat,
                    const Kernel::DepthImage* d,
                    const Kernel::NormalImage* n)
    : mat(mat), depth(QOpenGLTexture::Target2D),
      norm(QOpenGLTexture::Target2D)
{
    depth.setFormat(QOpenGLTexture::D32F);
    norm.setFormat(QOpenGLTexture::RGBA8_UNorm);
    for (auto tex : {&depth, &norm})
    {
        tex->setSize(d->rows(), d->cols());
        tex->setAutoMipMapGenerationEnabled(false);
        tex->allocateStorage();
    }

    depth.setData(QOpenGLTexture::Depth, QOpenGLTexture::Float32, d->data());
    norm.setData(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, n->data());

    assert(depth.isCreated());
    assert(norm.isCreated());
}

////////////////////////////////////////////////////////////////////////////////

Blitter::QuadAdd::QuadAdd(
        Blitter* parent, Renderer* R, const QMatrix4x4& mat,
        const Kernel::DepthImage* d, const Kernel::NormalImage* n)
    : parent(parent), R(R), mat(mat), depth(d), norm(n)
{
    // Nothing to do here
}

void Blitter::QuadAdd::run()
{
    delete parent->quads[R];
    parent->quads[R] = new Quad(mat, depth, norm);

    delete depth;
    delete norm;

    emit(parent->changed());
}

Blitter::QuadForget::QuadForget(Blitter* parent, Renderer* R)
    : parent(parent), R(R)
{
    // Nothing to do here
}

void Blitter::QuadForget::run()
{
    delete parent->quads[R];
    parent->quads.erase(R);
    emit(parent->changed());
}


}   // namespace Render
}   // namespace App
