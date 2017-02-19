#pragma once

#include <QQuickFramebufferObject>

#include "graph/types/index.hpp"
#include "graph/types/keys.hpp"
#include "graph/root.hpp"

#include "kernel/tree/tree.hpp"
#include "kernel/bind/bind_s7.h"

#include "render/axes.hpp"
#include "render/renderer.hpp"
#include "render/blitter.hpp"
#include "render/picker.hpp"

namespace App {
namespace Render {

class Drag;

class Canvas : public QObject, public QQuickFramebufferObject::Renderer
{
    Q_OBJECT
public:
    Canvas();

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;
    void render() override;

public slots:
    ////////////////////////////////////////////////////////////////////////////
    //  Graph serialization
    //  (which we only track closely enough to get shapes)
    void push(const int instance_index, const QString& instance_name,
              const QString& sheet_name);
    void pop();
    void cell(int c, const QString& name, const QString& expr, int type,
              bool valid, const QString& val, void* ptr);

signals:
    void viewChanged(QMatrix4x4 mat, QSize size);

protected:
    typedef std::pair<Graph::CellKey, const Kernel::Bind::shape_t*> ShapeKey;

    /*
     *  Checks the shape, installing a new Renderer if necessary
     */
    void installShape(const ShapeKey& s);

    // Pull in matrix and size from parent
    void synchronize(QQuickFramebufferObject *item) override;

    // Properties used in drawing and rendering
    QMatrix4x4 M;
    QSize window_size;
    QVector2D mouse;

    /*  Here, we store the set of shapes to be drawn */
    std::map<ShapeKey, App::Render::Renderer*> shapes;

    /*  Used in graph serialization to keep track of which keys are active */
    std::list<Graph::InstanceIndex> env;
    std::set<ShapeKey> visited;

    /*  Draws the background axes  */
    Axes axes;
    /*  Responsible for actually blitting rendered shapes to the screen */
    Blitter blitter;
    /*  Used to pick items */
    Picker picker;
};

////////////////////////////////////////////////////////////////////////////////

class CanvasObject : public QQuickFramebufferObject
{
    Q_OBJECT
public:
    QQuickFramebufferObject::Renderer* createRenderer() const override;

    /*
     *  Complete transform matrix
     */
    QMatrix4x4 M() const { return proj() * view(); }

    ////////////////////////////////////////////////////////////////////////////
    //  Changes to window properties
    Q_INVOKABLE void rotateIncremental(float dx, float dy);
    Q_INVOKABLE void panIncremental(float dx, float dy);
    Q_INVOKABLE void zoomIncremental(float ds, float x, float y);

    //  Manage mouse events here by setting mouse state and calling update
    //  (which asks the canvas to synchronize)
    Q_INVOKABLE void mouseMove(float x, float y);
    Q_INVOKABLE void mouseClick(int button);
    Q_INVOKABLE void mouseRelease();

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

    // Viewport settings
    float scale=1;
    QVector3D center={0,0,0};
    float pitch=0;
    float yaw=0;

    enum { RELEASED, CLICK_LEFT, CLICK_RIGHT,
           DRAG_ROT, DRAG_PAN, DRAG_HANDLE } mouse_state;
    QVector2D mouse_pos;
    Drag* mouse_drag;

    friend class Canvas;
};

}   // namespace Render
}   // namespace App