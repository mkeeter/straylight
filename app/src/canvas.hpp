#pragma once

#include <QQuickFramebufferObject>

#include "graph/index.hpp"

class Canvas : public QQuickFramebufferObject::Renderer
{
public:
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;
    void render() override;

    void push(Graph::InstanceIndex i);
    void pop();

protected:
    std::list<Graph::InstanceIndex> env;
};

class CanvasObject : public QQuickFramebufferObject
{
public:
    QQuickFramebufferObject::Renderer* createRenderer() const override;
};

