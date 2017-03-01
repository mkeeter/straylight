#pragma once

#include <QQuickFramebufferObject>

#include "graph/types/index.hpp"
#include "graph/types/keys.hpp"
#include "graph/root.hpp"

#include "kernel/tree/tree.hpp"
#include "kernel/bind/bind_s7.h"

#include "app/render/axes.hpp"
#include "app/render/blitter.hpp"
#include "app/render/picker.hpp"

namespace App {
namespace Render {

class Canvas : public QObject, public QQuickFramebufferObject::Renderer
{
    Q_OBJECT
public:
    Canvas();

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;
    void render() override;

protected:
    /* Pull in matrix and size from parent */
    void synchronize(QQuickFramebufferObject *item) override;

    /* Properties used in drawing and rendering */
    QMatrix4x4 M;
    QSize window_size;
    QVector2D mouse;

    /*  Draws the background axes  */
    Axes axes;

    /*  Responsible for actually blitting rendered shapes to the screen */
    Blitter blitter;

    /*  Used to pick items */
    Picker picker;

    /*  Indicates that we should re-render the picker object
     *  (this is an expensive operation!)  */
    bool picker_changed = true;
};

}   // namespace Render
}   // namespace App
