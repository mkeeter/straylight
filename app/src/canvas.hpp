#pragma once

#include <QQuickFramebufferObject>

#include "graph/index.hpp"
#include "graph/keys.hpp"
#include "graph/root.hpp"

#include "kernel/tree/tree.hpp"

class Canvas : public QQuickFramebufferObject::Renderer
{
public:
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;
    void render() override;

    void push(Graph::InstanceIndex i);
    void pop();
    void cell(Graph::CellIndex c, const Graph::Root* r);

protected:
    std::list<Graph::InstanceIndex> env;
    std::map<Graph::CellKey, Kernel::Tree*> shapes;
};

class CanvasObject : public QQuickFramebufferObject
{
public:
    QQuickFramebufferObject::Renderer* createRenderer() const override;
};

