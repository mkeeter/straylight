#pragma once

#include <QQuickFramebufferObject>
#include <QVector3D>
#include <QMatrix4x4>

#include "graph/response.hpp"
#include "app/render/renderer.hpp"

namespace App {
namespace Render {

class Drag;
class Handle;

class Scene : public QQuickFramebufferObject
{
    Q_OBJECT
public:
    /*
     *  Constructor installs this object into the App::GraphModel
     */
    Scene();

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

    Q_INVOKABLE void updatePicker();

    /*
     *  Update the model state with the given response
     */
    void updateFrom(const Graph::Response& r);

    /*
     *  Sets the env variable and triggers a render update
     */
    void setEnv(Graph::Env env_);

    /*
     *  Checks whether the given cell should be drawn, given the
     *  active environment env.
     *
     *  io cells are also drawn in their parent environment, hence the flag.
     */
    static bool shouldDraw(const Graph::Env& env, const Graph::CellKey& k, bool io);

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
     *  Requests that all Renderer objects update themselves
     */
    void updateRenderer();

    /*
     *  Activates or deactivates a renderer for the given key
     *  (if one exists)
     */
    void checkRenderer(const Graph::CellKey& k);

    // Viewport settings
    float scale=1;
    QVector3D center={0,0,0};
    float pitch=0;
    float yaw=0;

    enum { RELEASED, CLICK_LEFT, CLICK_RIGHT,
           DRAG_ROT, DRAG_PAN, DRAG_HANDLE } mouse_state;
    QVector2D mouse_pos;
    Drag* mouse_drag;

    /*  Indicates that the Canvas should re-render its picker image  */
    bool picker_changed = false;

    /*  Here, we store the set of shapes to be drawn */
    std::map<Graph::CellKey, App::Render::Renderer*> shapes;

    /*  This maps a particular cell to all the CellKeys that it is used in,
     *  so when a cell is erased we can erase all of its shapes  */
    std::map<Graph::CellIndex, std::set<Graph::CellKey>> cells;

    /*  This typedef reflects a cell + handle tag  */
    std::map<Graph::CellKey, Handle*> handles;

    /*  Env for drawing
     *  We only render Handles that are in or above the given env  */
    Graph::Env env;

    friend class Canvas;
};

}   // namespace Render
}   // namespace App
