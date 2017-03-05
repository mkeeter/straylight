#pragma once

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>

#include "app/render/handle.hpp"
#include "app/render/renderer.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace App {
namespace Render {

////////////////////////////////////////////////////////////////////////////////

class ShapeHandle : public Handle
{
public:
    /*
     *  Draws this handle
     */
    void _draw(const QMatrix4x4& world, const QMatrix4x4& proj,
               Picker::DrawMode mode, QRgb color) override;

    /*
     *  Updates the Tree from a given EscapedShape
     *
     *  Note that this doesn't give us new textures; those come from
     *  updateTexture (which should be hooked up to a Renderer)
     */
    bool updateFrom(App::Bridge::EscapedHandle* h) override;

    /*
     *  Updates the picker texture
     */
    void updateTexture(Renderer::Result* imgs);

    /*
     *  Returns a drag pointer
     *  The drag pointer escapes from render thread
     */
    Drag* getDrag(const QMatrix4x4& M, const QVector2D& cursor) override;

    /*  Used to disambiguate between Handle types  */
    int tag() const override { return _tag; }
    static const int _tag=2;

protected:
    /*
     *  Overloaded by derived classes to build VBOs, etc
     */
    void initGL() override;

    static QOpenGLBuffer* vbo;
    static QOpenGLVertexArrayObject* vao;
    static QOpenGLShaderProgram* shader;

    QScopedPointer<QOpenGLTexture> depth;
    QScopedPointer<QOpenGLTexture> norm;

    /*  Depth and normal images are claimed from Renderer::Result  */
    std::unique_ptr<Kernel::DepthImage> _depth;
    std::unique_ptr<Kernel::NormalImage> _norm;

    /*  Local transform matrix for depth / norm textures */
    QMatrix4x4 mat;

    /*  Our unique copy of the evaluator
     *  (purloined from the EscapedShape)  */
    std::unique_ptr<Kernel::Evaluator> eval;

    /*  Map from interpreter thread to local vars  */
    boost::bimap<Kernel::Cache::VarId, Kernel::Cache::VarId> vars;
};

}   // namespace Render
}   // namespace App
