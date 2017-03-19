#pragma once

#include <QVector3D>

#include "app/render/drag.hpp"
#include "app/bridge/escaped.hpp"

#include "kernel/eval/evaluator.hpp"

namespace App {
namespace Render {

class ShapeDrag : public Drag
{
public:
    /*
     *  Loads a starting position for a drag operation
     *
     *  p is in world coordinates
     *  This finds the specialized tree and the drag normal
     */
    void startDrag(const QVector3D& p);

    /*
     *  Drags to a particular position
     */
    Kernel::Solver::Solution dragTo(const QMatrix4x4& M,
                                    const QVector2D& cursor) override;

    /*
     *  Updates the solver tree
     */
    bool updateFrom(App::Bridge::EscapedShape* pt);

protected:
    /*  This is the full shape  */
    std::unique_ptr<Kernel::Evaluator> shape;

    /*  This is the shape with all branches collapsed, depending on
     *  the traversal path through the DAG when evaluated at the
     *  starting point  */
    std::unique_ptr<Kernel::Evaluator> specialized;

    /*  Here are our local variable values  */
    std::map<Kernel::Tree::Tree_*, float> vars;

    /*  We store the starting position for a drag here.
     *  This is a point in world space, and depends on where
     *  the user clicked on the shape  */
    QVector3D start;

    /*  This is the drag normal.  It is in world space and depends on
     *  the normal where the user clicked on the shape  */
    QVector3D norm;
};

}   // namespace Render
}   // namespace App
