
#include "app/render/shape_handle.hpp"
#include "app/bridge/escaped.hpp"

namespace App {
namespace Render {

QOpenGLBuffer* ShapeHandle::vbo = nullptr;
QOpenGLVertexArrayObject* ShapeHandle::vao = nullptr;
QOpenGLShaderProgram* ShapeHandle::shader = nullptr;

void ShapeHandle::_draw(const QMatrix4x4& world, const QMatrix4x4& proj,
                        Picker::DrawMode mode, QRgb color)
{
    // If the textures haven't been allocatd yet, then return immediately
    if (!depth.data())
    {
        return;
    }

    if (mode != Picker::DRAW_NORMAL)
    {
        return;
    }
    // TODO
    (void)color;

    QMatrix4x4 M = world;

    shader->bind();
    glUniformMatrix4fv(shader->uniformLocation("m"), 1, GL_FALSE, M.data());
    auto mquad = shader->uniformLocation("m_quad");

    glUniform1i(shader->uniformLocation("depth"), 0);
    glUniform1i(shader->uniformLocation("norm"), 1);

    vao->bind();
    depth->bind(0);
    norm->bind(1);
    glUniformMatrix4fv(mquad, 1, GL_FALSE, mat.data());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    vao->release();
    shader->release();

    // Reset active texture to avoid corrupting QML state
    glActiveTexture(GL_TEXTURE0);
}

bool ShapeHandle::updateFrom(App::Bridge::EscapedHandle* h)
{
    auto s = dynamic_cast<Bridge::EscapedShape*>(h);
    assert(s);

    eval.reset(s->eval);
    return false;
}

void ShapeHandle::updateTexture(const Renderer::Result* imgs)
{
    if (!depth.data())
    {
        /*  Construct textures  */
        depth.reset(new QOpenGLTexture(QOpenGLTexture::Target2D));
        norm.reset(new QOpenGLTexture(QOpenGLTexture::Target2D));
        depth->setFormat(QOpenGLTexture::D32F);
        norm->setFormat(QOpenGLTexture::RGBA8_UNorm);
    }
    depth->setData(QOpenGLTexture::Depth, QOpenGLTexture::Float32, imgs->depth.get());
    norm->setData(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, imgs->norm.get());
    mat = imgs->M;
}

Drag* ShapeHandle::getDrag(const QMatrix4x4& M, const QVector2D& cursor)
{
    return nullptr;
}

void ShapeHandle::initGL()
{
    if (vbo == nullptr)
    {
        vbo = new QOpenGLBuffer;
        vao = new QOpenGLVertexArrayObject;
        shader = new QOpenGLShaderProgram;

        shader->addShaderFromSourceFile(
                QOpenGLShader::Vertex, ":/gl/depthquad.vert");
        shader->addShaderFromSourceFile(
                QOpenGLShader::Fragment, ":/gl/depthquad.frag");
        shader->link();

        // Data is arranged  x   y
        GLfloat data[] = {  -1, -1,
                             1, -1,
                             1,  1,
                            -1,  1, };
        vbo->create();
        vbo->bind();
        vbo->allocate(data, sizeof(data));

        vao->create();
        vao->bind();

        glVertexAttribPointer(
                0, 2, GL_FLOAT, GL_FALSE,
                2 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        vbo->release();
        vao->release();
    }
}

}   // namespace Render
}   // namespace App
