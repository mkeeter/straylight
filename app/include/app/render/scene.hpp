#pragma once

#include <QQuickFramebufferObject>
#include <QVector3D>
#include <QMatrix4x4>

namespace App {
namespace Render {

class Drag;

class Scene : public QQuickFramebufferObject
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

    Q_INVOKABLE void updatePicker();

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

    /*  Indicates that the Canvas should re-render its picker image  */
    bool picker_changed = false;

    friend class Canvas;
};

}   // namespace Render
}   // namespace App
