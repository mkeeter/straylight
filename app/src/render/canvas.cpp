#include <QOpenGLFramebufferObject>

#include "app/render/canvas.hpp"
#include "app/render/scene.hpp"
#include "app/render/handle.hpp"

#include "kernel/bind/bind_s7.h"

namespace App {
namespace Render {

Canvas::Canvas()
{
    connect(&blitter, &Blitter::changed, [=](){ this->update(); });
    connect(this, &Canvas::viewChanged, &picker, &Picker::onViewChanged);
}

QOpenGLFramebufferObject* Canvas::createFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);

    return new QOpenGLFramebufferObject(size, format);
}

void Canvas::render()
{
    glClearColor(0, 0, 0, 1);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // Draw the picker layer (with a dummy mouse position)
    if (picker_changed)
    {
        picker.draw({0,0}, Picker::DRAW_PICKER);
        picker.setImage(framebufferObject()->toImage(false));
        picker_changed = false;
    }

    //  toImage releases binding for some reason
    framebufferObject()->bind();
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // Draw all of the scene objects
    blitter.draw(M);
    axes.draw(M);
    picker.draw(mouse.toPoint());
}

void Canvas::gotRenderer(Graph::CellKey k, App::Render::Renderer* r)
{
    if (shapes.count(k))
    {
        shapes.at(k)->deleteWhenNotRunning();
    }

    shapes[k] = r;

    // Connect the renderer to our viewChanged signal so it will
    // automatically redraw when necessary
    connect(this, &Canvas::viewChanged,
            r, &App::Render::Renderer::onViewChanged);

    // Connect the renderer and the blitter, so bitmaps will
    // magically appear in the 3D viewport
    connect(r, &App::Render::Renderer::gotResult,
            &blitter, &Blitter::addQuad, Qt::DirectConnection);
    connect(r, &App::Render::Renderer::goodbye,
            &blitter, &Blitter::forget);

    // Kick off an initial render
    r->onViewChanged(M, window_size);
}

void Canvas::synchronize(QQuickFramebufferObject *item)
{
    auto c = dynamic_cast<Scene*>(item);
    assert(c);

    picker_changed = c->picker_changed;
    c->picker_changed = false;

    const auto M_ = c->M();
    const QSize window_size_(c->width(), c->height());

    // Set mouse position, adjusting for high-DPI framebuffer
    mouse = c->mouse_pos * (picker.width() / c->width());
    QVector2D mouse_gl(
        2 * c->mouse_pos.x() / c->width() - 1,
        2 * c->mouse_pos.y() / c->height() - 1);

    // Handle mouse state machine based on picker state
    if (c->mouse_state == c->CLICK_LEFT)
    {
        if (auto h = picker.pickAt(mouse.toPoint()))
        {
            c->mouse_state = c->DRAG_HANDLE;
            c->mouse_drag = h->getDrag(M.inverted(), mouse_gl);
        }
        else
        {
            c->mouse_state = c->DRAG_ROT;
        }
    }
    else if (c->mouse_state == c->CLICK_RIGHT)
    {
        c->mouse_state = c->DRAG_PAN;
    }

    const bool view_changed = (M != M_) || (window_size != window_size_);

    if (view_changed)
    {
        M = M_;
        window_size = window_size_;
        emit(viewChanged(M, window_size));
    }
}

}   // namespace Render
}   // namespace App
