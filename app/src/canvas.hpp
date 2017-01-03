#pragma once

#include <QQuickFramebufferObject>

class Canvas : public QQuickFramebufferObject::Renderer
{
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void render();
};

class CanvasObject : public QQuickFramebufferObject
{
    QQuickFramebufferObject::Renderer* createRenderer() const override;
};

