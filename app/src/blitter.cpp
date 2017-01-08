#include "blitter.hpp"

Blitter::Blitter()
{
    initializeOpenGLFunctions();

    shader.addShaderFromSourceFile(
            QOpenGLShader::Vertex, ":/gl/depthquad.vert");
    shader.addShaderFromSourceFile(
            QOpenGLShader::Fragment, ":/gl/depthquad.frag");
    shader.link();

    GLfloat data[] ={   -1.0f, -1.0f,
                         1.0f, -1.0f,
                         1.0f,  1.0f,
                        -1.0f,  1.0f };

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

void Blitter::addQuad(Renderer* R, Renderer::Result imgs, Renderer::Task t)
{
    (void)R;
    (void)imgs;
    (void)t;

    quads[R] = Quad();
}

void Blitter::forget(Renderer* R)
{
    quads.erase(R);
}

void Blitter::draw(QMatrix4x4 M)
{
    shader.bind();
    glUniformMatrix4fv(shader.uniformLocation("m"), 1, GL_FALSE, M.data());

    quad_vao.bind();
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    quad_vao.release();

    shader.release();
}
