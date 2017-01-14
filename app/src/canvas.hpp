#pragma once

#include <QQuickFramebufferObject>

#include "graph/index.hpp"
#include "graph/keys.hpp"
#include "graph/root.hpp"

#include "kernel/tree/tree.hpp"

#include "axes.hpp"
#include "renderer.hpp"
#include "blitter.hpp"

struct Shape;

class Canvas : public QObject, public QQuickFramebufferObject::Renderer
{
    Q_OBJECT
public:
    Canvas();

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

signals:
    void viewChanged(QMatrix4x4 mat, QSize size);

protected:
    /*
     *  Returns the projection matrix
     *  (which compensates for window aspect ratio)
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

    QSize window_size;

    // Background axes
    Axes axes;

    // Render tracking
    std::list<Graph::InstanceIndex> env;

    /*  Here, we store the set of shapes to be drawn */
    typedef std::pair<Graph::CellKey, const Shape*> ShapeKey;
    std::map<ShapeKey, ::Renderer*> shapes;

    /*  Used in graph serialization to keep track of which keys are active */
    std::set<ShapeKey> visited;

    /*  Responsible for actually blitting rendered shapes to the screen */
    Blitter blitter;
};

////////////////////////////////////////////////////////////////////////////////

class CanvasObject : public QQuickFramebufferObject
{
public:
    QQuickFramebufferObject::Renderer* createRenderer() const override;
};
