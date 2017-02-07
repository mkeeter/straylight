#include "render/axes.hpp"

namespace App {
namespace Render {

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

#define MATERIAL_RED    0.957, 0.263, 0.212
#define MATERIAL_GREEN  0.298, 0.686, 0.314
#define MATERIAL_BLUE   0.129, 0.588, 0.953
        // Data is arranged  x   y   z   r   g   b
        GLfloat data[] = {   /********************/
                             /*    X axis        */
                             o,  o,  o,  MATERIAL_RED,
                             o, -o,  o,  MATERIAL_RED,
                             1,  0,  0,  MATERIAL_RED,

                             o,  o, -o,  MATERIAL_RED,
                             o, -o, -o,  MATERIAL_RED,
                             1,  0,  0,  MATERIAL_RED,

                             o,  o,  o,  MATERIAL_RED,
                             o,  o, -o,  MATERIAL_RED,
                             1,  0,  0,  MATERIAL_RED,

                             o, -o,  o,  MATERIAL_RED,
                             o, -o, -o,  MATERIAL_RED,
                             1,  0,  0,  MATERIAL_RED,
                             /********************/
                             /*    Y axis        */
                             o,  o,  o,  MATERIAL_GREEN,
                            -o,  o,  o,  MATERIAL_GREEN,
                             0,  1,  0,  MATERIAL_GREEN,

                             o,  o, -o,  MATERIAL_GREEN,
                            -o,  o, -o,  MATERIAL_GREEN,
                             0,  1,  0,  MATERIAL_GREEN,

                             o,  o,  o,  MATERIAL_GREEN,
                             o,  o, -o,  MATERIAL_GREEN,
                             0,  1,  0,  MATERIAL_GREEN,

                            -o,  o,  o,  MATERIAL_GREEN,
                            -o,  o, -o,  MATERIAL_GREEN,
                             0,  1,  0,  MATERIAL_GREEN,
                             /********************/
                             /*    Z axis        */
                             o,  o,  o,  MATERIAL_BLUE,
                            -o,  o,  o,  MATERIAL_BLUE,
                             0,  0,  1,  MATERIAL_BLUE,

                             o, -o,  o,  MATERIAL_BLUE,
                            -o, -o,  o,  MATERIAL_BLUE,
                             0,  0,  1,  MATERIAL_BLUE,

                             o,  o,  o,  MATERIAL_BLUE,
                             o, -o,  o,  MATERIAL_BLUE,
                             0,  0,  1,  MATERIAL_BLUE,

                            -o,  o,  o,  MATERIAL_BLUE,
                            -o, -o,  o,  MATERIAL_BLUE,
                             0,  0,  1,  MATERIAL_BLUE,
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
    solid_vao.release();

    shader.release();
}

}   // namespace Render
}   // namespace App
