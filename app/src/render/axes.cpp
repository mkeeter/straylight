#include "app/render/axes.hpp"
#include "app/ui/material.hpp"

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

#define MATERIAL(m)     m.red()/255.0f, m.green()/255.0f, m.blue()/255.0f
#define MATERIAL_RED    MATERIAL(App::UI::Material::red)
#define MATERIAL_GREEN  MATERIAL(App::UI::Material::green)
#define MATERIAL_BLUE   MATERIAL(App::UI::Material::blue)
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

    {   // In-line data for wireframe model
        // Data is arranged  x   y   z   r   g   b
        GLfloat data[] = {   0,  0,  0,  MATERIAL_RED,
                             1,  0,  0,  MATERIAL_RED,
                             0,  0,  0,  MATERIAL_GREEN,
                             0,  1,  0,  MATERIAL_GREEN,
                             0,  0,  0,  MATERIAL_BLUE,
                             0,  0,  1,  MATERIAL_BLUE,
        };
        wire_vbo.create();
        wire_vbo.bind();
        wire_vbo.allocate(data, sizeof(data));

        wire_vao.create();
        wire_vao.bind();

        // Data stored in VAO
        glVertexAttribPointer(
                0, 3, GL_FLOAT, GL_FALSE,
                6 * sizeof(GLfloat), (GLvoid*)0);
        glVertexAttribPointer(
                1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        wire_vbo.release();
        wire_vao.release();
    }
}

void Axes::drawSolid(QMatrix4x4 M)
{
    shader.bind();
    glUniformMatrix4fv(shader.uniformLocation("m"), 1, GL_FALSE, M.data());

    solid_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 54);
    solid_vao.release();

    shader.release();
}

void Axes::drawWire(QMatrix4x4 M)
{
    shader.bind();
    glUniformMatrix4fv(shader.uniformLocation("m"), 1, GL_FALSE, M.data());

    glDisable(GL_DEPTH_TEST);
    wire_vao.bind();
    glDrawArrays(GL_LINES, 0, 6);
    glEnable(GL_DEPTH_TEST);

    shader.release();
}

}   // namespace Render
}   // namespace App
