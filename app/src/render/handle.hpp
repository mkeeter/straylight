#pragma once

#include <QMatrix4x4>
#include <QOpenGLFunctions>

#include "graph/ptr.hpp"

namespace App {
namespace Render {

class Handle : public QOpenGLFunctions
{
public:
    virtual ~Handle() { /* Nothing to do here */ }

    enum DrawMode { PICKER, BASE, HOVER, DRAG };

    /*
     *  Initializes OpenGL functions if not ready, then calls _draw
     */
    void draw(const QMatrix4x4& world, const QMatrix4x4& proj,
              Handle::DrawMode mode);

    /*
     *  Draws this handle
     */
    virtual void _draw(const QMatrix4x4& world, const QMatrix4x4& proj,
                       Handle::DrawMode mode)=0;

    /*
     *  Updates the handle's data
     *
     *  Returns true if things have changed
     */
    virtual bool updateFrom(Graph::ValuePtr p)=0;

protected:
    /*
     *  Overloaded by derived classes to build VBOs, etc
     */
    virtual void initGL() { }

    /*  Stores whether we've called initalizeOpenGLFunctions */
    bool gl_ready = false;
};

}   // namespace Render
}   // namespace App
