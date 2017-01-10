#pragma once

#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>

#include "renderer.hpp"

class Blitter : public QObject, public QOpenGLFunctions
{
    Q_OBJECT
public:
    Blitter();
    void draw(QMatrix4x4 M);

public slots:
    void addQuad(Renderer* R, Renderer::Result imgs, Renderer::Task t);
    void forget(Renderer*);

protected:
    struct Quad {
        Quad(const QMatrix4x4& mat, const Kernel::DepthImage& d,
             const Kernel::NormalImage& n);
        QMatrix4x4 mat;
        QOpenGLTexture depth;
        QOpenGLTexture norm;
    };
    std::map<Renderer*, Quad*> quads;

    QOpenGLBuffer quad_vbo;
    QOpenGLVertexArrayObject quad_vao;
    QOpenGLShaderProgram shader;
};
