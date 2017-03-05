#include <QOpenGLFramebufferObject>

#include "app/render/canvas.hpp"
#include "app/render/scene.hpp"
#include "app/render/handle.hpp"
#include "app/render/shape_handle.hpp"

#include "kernel/bind/bind_s7.h"

namespace App {
namespace Render {

Canvas::Canvas()
{
    // Nothing to do here
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

        //  toImage releases binding for some reason
        framebufferObject()->bind();
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    }

    // Draw all of the scene objects
    axes.drawSolid(M);
    picker.draw(mouse.toPoint());
    axes.drawWire(M);
}

void Canvas::synchronize(QQuickFramebufferObject *item)
{
    auto scene = dynamic_cast<Scene*>(item);
    assert(scene);

    //////////////////////////////////////////////////////////////////////////
    // Claim picker_changed from the parent scene
    if (scene->mouse_state == Scene::RELEASED)
    {
        picker_changed = scene->picker_changed;
        scene->picker_changed = false;
    }

    const auto M_ = scene->M();
    const QSize window_size_(scene->width(), scene->height());

    // Set mouse position, adjusting for high-DPI framebuffer
    mouse = scene->mouse_pos * (picker.width() / scene->width());
    QVector2D mouse_gl(
        2 * scene->mouse_pos.x() / scene->width() - 1,
        2 * scene->mouse_pos.y() / scene->height() - 1);

    //////////////////////////////////////////////////////////////////////////
    // Handle mouse state machine based on picker state
    if (scene->mouse_state == scene->CLICK_LEFT)
    {
        if (auto h = picker.pickAt(mouse.toPoint()))
        {
            scene->mouse_state = scene->DRAG_HANDLE;
            scene->mouse_drag = h->getDrag(M.inverted(), mouse_gl);
        }
        else
        {
            scene->mouse_state = scene->DRAG_ROT;
        }
    }
    else if (scene->mouse_state == scene->CLICK_RIGHT)
    {
        scene->mouse_state = scene->DRAG_PAN;
    }

    //////////////////////////////////////////////////////////////////////////
    //  Update view parameters
    const bool view_changed = (M != M_) || (window_size != window_size_);

    if (view_changed)
    {
        M = M_;
        window_size = window_size_;
        picker.setView(M, window_size);
    }

    //////////////////////////////////////////////////////////////////////////
    //  Update blitter with new shapes
    for (const auto& s : scene->shapes)
    {
        auto result = s.second->getResult();
        if (result)
        {
            if (auto h = picker.getHandle(s.first))
            {
                auto s = dynamic_cast<ShapeHandle*>(h);
                assert(s);
                s->updateTexture(result);
            }
            delete result;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    std::set<Graph::CellKey> hs;
    for (const auto& h : scene->handles)
    {
        picker.installHandle(h.first, h.second);
        hs.insert(h.first);
    }
    picker.prune(hs);
}

}   // namespace Render
}   // namespace App
