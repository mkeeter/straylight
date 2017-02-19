#pragma once

#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QColor>

#include "graph/types/ptr.hpp"

#include "app/render/picker.hpp"
#include "app/render/drag.hpp"

namespace App {
namespace Render {

class Drag;

class Handle : public QOpenGLFunctions
{
public:
    /*
     *  Claims ownership of the given Drag object
     */
    Handle(std::unique_ptr<Drag>& drag);
    virtual ~Handle() { /* Nothing to do here */ }

    /*
     *  Initializes OpenGL functions if not ready, then calls _draw
     */
    void draw(const QMatrix4x4& world, const QMatrix4x4& proj,
              Picker::DrawMode mode=Picker::DRAW_NORMAL, QRgb color=0);

    /*
     *  Draws this handle
     */
    virtual void _draw(const QMatrix4x4& world, const QMatrix4x4& proj,
                       Picker::DrawMode mode, QRgb color)=0;

    /*
     *  Updates the handle's data
     *
     *  Returns true if things have changed
     */
    virtual bool updateFrom(Graph::ValuePtr p)=0;

    /*
     *  Returns a drag pointer
     *  The drag pointer escapes from render thread
     */
    Drag* getDrag(const QMatrix4x4& M, const QVector2D& cursor);

protected:
    /*
     *  Overloaded by derived classes to build VBOs, etc
     */
    virtual void initGL() { }

    /*
     *  Returns the current position of the drag handle
     *  (used when setting initial drag parameters in getDrag)
     */
    virtual QVector3D pos(const QMatrix4x4& M, const QVector2D& cursor) const=0;

    /*  Stores whether we've called initalizeOpenGLFunctions */
    bool gl_ready = false;

    std::unique_ptr<Drag> drag;
};

}   // namespace Render
}   // namespace App
