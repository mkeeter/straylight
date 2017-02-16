#pragma once

#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QColor>

#include "graph/ptr.hpp"

#include "render/picker.hpp"
#include "render/drag.hpp"

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
    Drag* getDrag() { return drag.get(); }

protected:
    /*
     *  Overloaded by derived classes to build VBOs, etc
     */
    virtual void initGL() { }

    /*  Stores whether we've called initalizeOpenGLFunctions */
    bool gl_ready = false;

    std::unique_ptr<Drag> drag;
};

}   // namespace Render
}   // namespace App
