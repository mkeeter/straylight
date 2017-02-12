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

void PointHandle::_draw(const QMatrix4x4& m, bool selected)
{
    (void)selected;

    shader.bind();
    glUniformMatrix4fv(shader.uniformLocation("m"), 1, GL_FALSE, m.data());
    glUniform3f(shader.uniformLocation("p"), center.x(), center.y(), center.z());

    vao.bind();
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
    vao.release();
    shader.release();
}

bool PointHandle::updateFrom(Graph::ValuePtr ptr)
{
    auto p = App::Bind::get_point_handle(ptr);
    QVector3D c(p->pos[0], p->pos[1], p->pos[2]);

    qDebug() << center << c;
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
        shader.addShaderFromSourceFile(
                QOpenGLShader::Vertex, ":/gl/point_handle.vert");
        shader.addShaderFromSourceFile(
                QOpenGLShader::Fragment, ":/gl/point_handle.frag");
        shader.link();

        {
            std::vector<GLfloat> data;
            data.push_back(0);
            data.push_back(0);
            for (int i=0; i <= segments; ++i)
            {
                data.push_back(cos(i / 16.0f * 2 * pi));
                data.push_back(sin(i / 16.0f * 2 * pi));
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
