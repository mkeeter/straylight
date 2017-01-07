#pragma once

#include <QQuickFramebufferObject>

#include "graph/index.hpp"
#include "graph/keys.hpp"
#include "graph/root.hpp"

#include "kernel/tree/tree.hpp"

#include "axes.hpp"
#include "renderer.hpp"

struct Shape;

class Canvas : public QObject, public QQuickFramebufferObject::Renderer
{
    Q_OBJECT
public:
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;
    void render() override;

    void push(Graph::InstanceIndex i);
    void pop();
    void cell(Graph::CellIndex c, const Graph::Root* r);

public slots:
    void setSize(float w, float h);
    void rotateIncremental(float dx, float dy);
    void panIncremental(float dx, float dy);
    void zoomIncremental(float ds, float x, float y);

protected:
    /*
     *  Returns the projection matrix
     *  (which compensates for window size)
     */
    QMatrix4x4 proj() const;

    /*
     *  Returns the view matrix
     *  (with rotation, scale, and center applied)
     */
    QMatrix4x4 view() const;

    /*
     *  Complete transform matrix
     */
    QMatrix4x4 M() const { return proj() * view(); }

    // Viewport settings
    float scale=1;
    QVector3D center={0,0,0};
    float pitch=0;
    float yaw=0;

    QVector2D window_size;

    // Background axes
    Axes axes;

    // Render tracking
    std::list<Graph::InstanceIndex> env;

    std::set<std::pair<Graph::CellKey, const Shape*>> visited;
    std::map<std::pair<Graph::CellKey, const Shape*>, ::Renderer*> shapes;
};

class CanvasObject : public QQuickFramebufferObject
{
public:
    QQuickFramebufferObject::Renderer* createRenderer() const override;
    void onWidthChanged(float w);
};
