
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

    // Only draw terminal values
    if (!is_endpoint)
    {
        return;
    }

    (void)proj;

    QMatrix4x4 M = world;

    shader->bind();
    glUniformMatrix4fv(shader->uniformLocation("m"), 1, GL_FALSE, M.data());
    auto mquad = shader->uniformLocation("m_quad");

    glUniform1i(shader->uniformLocation("depth"), 0);
    glUniform1i(shader->uniformLocation("norm"), 1);

    // Load mode and picker color in case this is a pick render
    glUniform1i(shader->uniformLocation("draw_mode"), mode);
    glUniformColor3f(*shader, "picker_color", color);

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

    drag.updateFrom(s);
    return false;
}

void ShapeHandle::updateTexture(Renderer::Result* imgs)
{
    if (!depth.data())
    {
        /*  Construct textures  */
        depth.reset(new QOpenGLTexture(QOpenGLTexture::Target2D));
        norm.reset(new QOpenGLTexture(QOpenGLTexture::Target2D));
    }

    for (auto tex : {depth.data(), norm.data()})
    {
        if (tex->width() != imgs->depth->cols() ||
            tex->height() != imgs->depth->rows())
        {
            tex->destroy();
            tex->setFormat(tex == depth.data() ? QOpenGLTexture::D32F
                                               : QOpenGLTexture::RGBA8_UNorm);
            tex->setSize(imgs->depth->rows(), imgs->depth->cols());
            tex->setAutoMipMapGenerationEnabled(false);
            tex->allocateStorage();
        }
    }
    assert(depth->isCreated());
    assert(norm->isCreated());

    depth->setData(QOpenGLTexture::Depth, QOpenGLTexture::Float32, imgs->depth->data());
    norm->setData(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, imgs->norm->data());

    _depth.reset(imgs->depth.release());
    _norm.reset(imgs->norm.release());

    mat = imgs->M;
}

Drag* ShapeHandle::getDrag(const QMatrix4x4& M, const QVector2D& cursor)
{
    const auto c = (cursor + QVector2D(1, 1)) / 2;
    const int x = c.x() * _depth->cols();
    const int y = c.y() * _depth->rows();
    const float depth = 2 * ((*_depth)(y, x) - 0.5);

    const QVector3D pos_screen(cursor.x(), cursor.y(), depth);
    const QVector3D pos = M * pos_screen;

    drag.startDrag(pos);
    return &drag;
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
