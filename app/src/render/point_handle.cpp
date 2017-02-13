#include <boost/math/constants/constants.hpp>

#include "render/point_handle.hpp"

using namespace boost::math::float_constants;

namespace App {
namespace Render {

////////////////////////////////////////////////////////////////////////////////

PointHandle::PointHandle()
{
    // Nothing to do here
}

void PointHandle::setVars(const QMatrix4x4& world, const QMatrix4x4& proj,
                          QOpenGLShaderProgram& shader)
{
    glUniformMatrix4fv(shader.uniformLocation("m_world"), 1, GL_FALSE, world.data());
    glUniformMatrix4fv(shader.uniformLocation("m_proj"), 1, GL_FALSE, proj.data());
    glUniform3f(shader.uniformLocation("pos"), center.x(), center.y(), center.z());
}

void PointHandle::_draw(const QMatrix4x4& world, const QMatrix4x4& proj,
                        Handle::DrawMode mode)
{
    (void)mode;
    vao.bind();

    shader_solid.bind();
    setVars(world, proj, shader_solid);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
    shader_solid.release();

    shader_dotted.bind();
    setVars(world, proj, shader_dotted);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
    glEnable(GL_DEPTH_TEST);

    vao.release();
    shader_dotted.release();
}

bool PointHandle::updateFrom(Graph::ValuePtr ptr)
{
    auto p = App::Bind::get_point_handle(ptr);
    QVector3D c(p->pos[0], p->pos[1], p->pos[2]);

    if (c != center)
    {
        center = c;
        return true;
    }
    else
    {
        return false;
    }
}

void PointHandle::initGL()
{
    if (!gl_ready)
    {
        shader_solid.addShaderFromSourceFile(
                QOpenGLShader::Vertex, ":/gl/point_handle.vert");
        shader_solid.addShaderFromSourceFile(
                QOpenGLShader::Fragment, ":/gl/point_handle.frag");
        shader_solid.link();

        shader_dotted.addShaderFromSourceFile(
                QOpenGLShader::Vertex, ":/gl/point_handle.vert");
        shader_dotted.addShaderFromSourceFile(
                QOpenGLShader::Fragment, ":/gl/point_handle_dotted.frag");
        shader_dotted.link();

        {
            std::vector<GLfloat> data;
            data.push_back(0);
            data.push_back(0);
            for (int i=0; i <= segments; ++i)
            {
                data.push_back(cos(2.0f * pi * i / segments));
                data.push_back(sin(2.0f * pi * i / segments));
            }
            vbo.create();
            vbo.bind();
            vbo.allocate(data.data(), data.size() * sizeof(GLfloat));
        }

        vao.create();
        vao.bind();

        glVertexAttribPointer(
                0, 2, GL_FLOAT, GL_FALSE,
                2 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        vbo.release();
        vao.release();

        gl_ready = true;
    }
}

}   // namespace Render
}   // namespace App
