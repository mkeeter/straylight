#pragma once

#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QColor>

#include "app/render/picker.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace App {

// Forward declaration
namespace Bridge { class EscapedHandle; }

namespace Render {

// Forward declaration
class Drag;

////////////////////////////////////////////////////////////////////////////////

class Handle : public QOpenGLFunctions
{
public:
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
    virtual bool updateFrom(App::Bridge::EscapedHandle* h)=0;

    /*
     *  Returns a drag pointer
     *  The drag pointer escapes from render thread
     */
    virtual Drag* getDrag(const QMatrix4x4& M, const QVector2D& cursor)=0;

    /*
     *  Every Handle should rederive this class differently
     */
    virtual int tag() const=0;

    /*
     * Helper functions to bind colors into shaders
     */
    void glUniformColor3f(QOpenGLShaderProgram& shader,
                          const QString& var, const QColor& color);
    void glUniformColor3f(QOpenGLShaderProgram& shader,
                          const QString& var, const QRgb color);

    /*
     *  Getter / setter for is_io
     */
    bool isIO() const { return is_io; }
    bool setIO(bool i) { bool c = (is_io != i); is_io = i; return c; }

    /*
     *  Getter / setter for is_endpoint
     */
    bool isEndpoint() const { return is_endpoint; }
    bool setEndpoint(bool i) { bool c = (is_endpoint != i); is_endpoint = i; return c; }

protected:
    /*
     *  Overloaded by derived classes to build VBOs, etc
     */
    virtual void initGL()=0;

    /*  Stores whether we've called initalizeOpenGLFunctions */
    bool gl_ready = false;

    /*  If true, this handle is exposed to the parent sheet as an I/O port
     *  This means it is drawn at a higher level of view environment  */
    bool is_io = false;

    /*  If true, this handle is the end of a computation chain
     *  (this determines whether shapes are rendered)  */
    bool is_endpoint = true;
};

}   // namespace Render
}   // namespace App
