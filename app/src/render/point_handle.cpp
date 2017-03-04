#include <boost/math/constants/constants.hpp>

#include "app/render/point_handle.hpp"
#include "app/ui/material.hpp"
#include "app/bridge/escaped.hpp"

using namespace boost::math::float_constants;

namespace App {
namespace Render {

QOpenGLBuffer* PointHandle::vbo;
QOpenGLVertexArrayObject* PointHandle::vao;
QOpenGLShaderProgram* PointHandle::shader_solid;
QOpenGLShaderProgram* PointHandle::shader_dotted;

////////////////////////////////////////////////////////////////////////////////

PointHandle::PointHandle()
    : drag(new PointDrag())
{
    // Nothing to do here
}

static void glUniformColor3f(QOpenGLShaderProgram& shader, const QString& var,
                             const QColor& color)
{
    glUniform3f(shader.uniformLocation(var),
                color.red()/255.0f, color.green()/255.0f, color.blue()/255.0f);
}

static void glUniformColor3f(QOpenGLShaderProgram& shader, const QString& var,
                             const QRgb color)
{
    glUniformColor3f(shader, var, QColor::fromRgb(color));
}

void PointHandle::setShaderVars(
        const QMatrix4x4& world, const QMatrix4x4& proj,
        QOpenGLShaderProgram& shader, Picker::DrawMode mode, QRgb color)
{
    glUniformMatrix4fv(shader.uniformLocation("m_world"), 1, GL_FALSE, world.data());
    glUniformMatrix4fv(shader.uniformLocation("m_proj"), 1, GL_FALSE, proj.data());
    glUniform3f(shader.uniformLocation("pos"), center.x(), center.y(), center.z());
    glUniform1f(shader.uniformLocation("scale"), 0.025f);

    switch (mode)
    {
        case Picker::DRAW_PICKER:
            glUniformColor3f(shader, "color_center", color);
            glUniformColor3f(shader, "color_edge", color);
            break;
        case Picker::DRAW_NORMAL:
            glUniformColor3f(shader, "color_center", App::UI::Material::blue_grey_200);
            glUniformColor3f(shader, "color_edge", App::UI::Material::blue_grey_500);
            break;
        case Picker::DRAW_HOVER:    // fallthrough
        case Picker::DRAW_DRAG:
            glUniformColor3f(shader, "color_center", App::UI::Material::blue_grey_100);
            glUniformColor3f(shader, "color_edge", App::UI::Material::blue_grey_400);
            break;
    }
}

void PointHandle::_draw(const QMatrix4x4& world, const QMatrix4x4& proj,
                        Picker::DrawMode mode, QRgb color)
{
    vao->bind();

    shader_solid->bind();
    setShaderVars(world, proj, *shader_solid, mode, color);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
    shader_solid->release();

    if (mode != Picker::DRAW_PICKER)
    {
        shader_dotted->bind();
        setShaderVars(world, proj, *shader_dotted, mode, color);
        glDisable(GL_DEPTH_TEST);
        glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
        glEnable(GL_DEPTH_TEST);
        shader_dotted->release();
    }

    vao->release();
}

bool PointHandle::updateFrom(App::Bridge::EscapedHandle* h)
{
    auto p = dynamic_cast<Bridge::EscapedPointHandle*>(h);
    assert(p);

    bool changed = (center != p->pos);
    center = p->pos;

    assert(drag.get());
    changed |= drag->updateFrom(p);

    return changed;
}

void PointHandle::initGL()
{
    if (!gl_ready)
    {
        if (vbo == nullptr)
        {
            vbo = new QOpenGLBuffer;
            vao = new QOpenGLVertexArrayObject;
            shader_solid = new QOpenGLShaderProgram;
            shader_dotted = new QOpenGLShaderProgram;

            shader_solid->addShaderFromSourceFile(
                    QOpenGLShader::Vertex, ":/gl/point_handle.vert");
            shader_solid->addShaderFromSourceFile(
                    QOpenGLShader::Fragment, ":/gl/point_handle.frag");
            shader_solid->link();

            shader_dotted->addShaderFromSourceFile(
                    QOpenGLShader::Vertex, ":/gl/point_handle.vert");
            shader_dotted->addShaderFromSourceFile(
                    QOpenGLShader::Fragment, ":/gl/point_handle_dotted.frag");
            shader_dotted->link();

            {
                std::vector<GLfloat> data;
                data.push_back(0);
                data.push_back(0);
                for (int i=0; i <= segments; ++i)
                {
                    data.push_back(cos(2.0f * pi * i / segments));
                    data.push_back(sin(2.0f * pi * i / segments));
                }
                vbo->create();
                vbo->bind();
                vbo->allocate(data.data(), data.size() * sizeof(GLfloat));
            }

            vao->create();
            vao->bind();

            glVertexAttribPointer(
                    0, 2, GL_FLOAT, GL_FALSE,
                    2 * sizeof(GLfloat), (GLvoid*)0);
            glEnableVertexAttribArray(0);

            vbo->release();
            vao->release();
        }

        gl_ready = true;
    }
}

Drag* PointHandle::getDrag(const QMatrix4x4& M, const QVector2D& cursor)
{
    (void)M;
    (void)cursor;

    drag->startDrag(center);
    return drag.get();
}

}   // namespace Render
}   // namespace App
