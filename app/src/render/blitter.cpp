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

void Blitter::addQuad(Renderer* R, const Renderer::Result imgs)
{
    delete quads[R];
    quads[R] = new Quad(imgs.M, imgs.depth, imgs.norm);

    delete imgs.depth;
    delete imgs.norm;
}

void Blitter::prune(const QSet<Renderer*>& rs)
{
    QSet<Renderer*> forgotten;
    for (auto q : quads)
    {
        if (rs.find(q.first) == rs.end())
        {
            delete q.second;
            forgotten.insert(q.first);
        }
    }

    for (auto q : forgotten)
    {
        quads.erase(q);
    }
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

}   // namespace Render
}   // namespace App
