#include "axes.hpp"

Axes::Axes()
{
    initializeOpenGLFunctions();

    shader.addShaderFromSourceFile(
            QOpenGLShader::Vertex, ":/gl/axes.vert");
    shader.addShaderFromSourceFile(
            QOpenGLShader::Fragment, ":/gl/axes.frag");
    shader.link();

    {   // In-line data for axes model
        const float o = 0.05;

        // Data is arranged  x   y   z   r   g   b
        GLfloat data[] = {   /********************/
                             /*    X axis        */
                             o,  o,  o,  1,  0,  0,
                             o, -o,  o,  1,  0,  0,
                             1,  0,  0,  1,  0,  0,

                             o,  o, -o,  1,  0,  0,
                             o, -o, -o,  1,  0,  0,
                             1,  0,  0,  1,  0,  0,

                             o,  o,  o,  1,  0,  0,
                             o,  o, -o,  1,  0,  0,
                             1,  0,  0,  1,  0,  0,

                             o, -o,  o,  1,  0,  0,
                             o, -o, -o,  1,  0,  0,
                             1,  0,  0,  1,  0,  0,
                             /********************/
                             /*    Y axis        */
                             o,  o,  o,  0,  1,  0,
                            -o,  o,  o,  0,  1,  0,
                             0,  1,  0,  0,  1,  0,

                             o,  o, -o,  0,  1,  0,
                            -o,  o, -o,  0,  1,  0,
                             0,  1,  0,  0,  1,  0,

                             o,  o,  o,  0,  1,  0,
                             o,  o, -o,  0,  1,  0,
                             0,  1,  0,  0,  1,  0,

                            -o,  o,  o,  0,  1,  0,
                            -o,  o, -o,  0,  1,  0,
                             0,  1,  0,  0,  1,  0,
                             /********************/
                             /*    Z axis        */
                             o,  o,  o,  0,  0,  1,
                            -o,  o,  o,  0,  0,  1,
                             0,  0,  1,  0,  0,  1,

                             o, -o,  o,  0,  0,  1,
                            -o, -o,  o,  0,  0,  1,
                             0,  0,  1,  0,  0,  1,

                             o,  o,  o,  0,  0,  1,
                             o, -o,  o,  0,  0,  1,
                             0,  0,  1,  0,  0,  1,

                            -o,  o,  o,  0,  0,  1,
                            -o, -o,  o,  0,  0,  1,
                             0,  0,  1,  0,  0,  1,
                             /********************/
                             /*    Base cube     */
                             o,  o, -o,  1,  1,  1,
                             o, -o, -o,  1,  1,  1,
                            -o,  o, -o,  1,  1,  1,

                            -o, -o,  o,  1,  1,  1,
                            -o,  o,  o,  1,  1,  1,
                            -o,  o, -o,  1,  1,  1,

                            -o, -o,  o,  1,  1,  1,
                             o, -o, -o,  1,  1,  1,
                             o, -o,  o,  1,  1,  1,

                            -o, -o, -o,  1,  1,  1,
                             o, -o, -o,  1,  1,  1,
                            -o,  o, -o,  1,  1,  1,

                            -o, -o,  o,  1,  1,  1,
                            -o, -o, -o,  1,  1,  1,
                            -o,  o, -o,  1,  1,  1,

                            -o, -o,  o,  1,  1,  1,
                             o, -o, -o,  1,  1,  1,
                            -o, -o, -o,  1,  1,  1,
        };
        solid_vbo.create();
        solid_vbo.bind();
        solid_vbo.allocate(data, sizeof(data));

        solid_vao.create();
        solid_vao.bind();

        // Data stored in VAO
        glVertexAttribPointer(
                0, 3, GL_FLOAT, GL_FALSE,
                6 * sizeof(GLfloat), (GLvoid*)0);
        glVertexAttribPointer(
                1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        solid_vbo.release();
        solid_vao.release();
    }
}

void Axes::draw(QMatrix4x4 M)
{
    shader.bind();
    glUniformMatrix4fv(shader.uniformLocation("m"), 1, GL_FALSE, M.data());

    solid_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 54);

    shader.release();
}
