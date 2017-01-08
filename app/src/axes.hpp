#pragma once

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

class Axes : public QOpenGLFunctions
{
public:
    Axes();
    void draw(QMatrix4x4 M);

protected:
    QOpenGLBuffer solid_vbo;
    QOpenGLVertexArrayObject solid_vao;

    QOpenGLShaderProgram shader;
};