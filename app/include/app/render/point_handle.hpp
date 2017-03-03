#pragma once

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

#include "app/render/handle.hpp"
#include "app/render/point_drag.hpp"

#include "app/bridge/escaped.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace App {
namespace Render {

class PointHandle : public Handle
{
public:
    PointHandle();

    /*
     *  Draws the point in the 3D viewport
     */
    void _draw(const QMatrix4x4& world, const QMatrix4x4& view,
               Picker::DrawMode mode, QRgb color=0) override;

    /*
     *  Updates from an EscapedPointHandle
     */
    bool updateFrom(App::Bridge::EscapedHandle* h) override;

    /*
     *  Returns a point dragger
     */
    Drag* getDrag(const QMatrix4x4& M, const QVector2D& cursor) override;

    /*  Used to disambiguate between Handle types */
    int tag() const override { return _tag; }
    static const int _tag=1;

protected:
    /*
     *  Constructs all of the OpenGL objects for this point handle
     */
    void initGL() override;

    /*
     *  Loads shader variables into the given program
     */
    void setShaderVars(const QMatrix4x4& world, const QMatrix4x4& proj,
                       QOpenGLShaderProgram& shader, Picker::DrawMode mode,
                       QRgb color);

    QVector3D center;

    // TODO: share these among all instances
    QScopedPointer<QOpenGLBuffer> vbo;
    QScopedPointer<QOpenGLVertexArrayObject> vao;
    QScopedPointer<QOpenGLShaderProgram> shader_solid;
    QScopedPointer<QOpenGLShaderProgram> shader_dotted;

    static const int segments=128;

    std::unique_ptr<PointDrag> drag;
};

}   // namespace Render
}   // namespace App
