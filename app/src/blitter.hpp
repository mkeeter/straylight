#pragma once
#include <QObject>
#include <QOpenGLTexture>

#include "renderer.hpp"

class Blitter : public QObject
{
    Q_OBJECT
public:
    Blitter() {}

public slots:
    void addQuad(Renderer* R, Renderer::Result imgs, Renderer::Task t);
    void forget(Renderer*);

protected:
    struct Quad {
        QVector3D center;
        QVector3D corner;
        //QOpenGLTexture depth;
        //QOpenGLTexture norm;
    };
    std::map<Renderer*, Quad> quads;
};
