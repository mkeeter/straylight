#include <QOpenGLFramebufferObject>

#include "app/render/canvas.hpp"
#include "app/render/handle.hpp"
#include "app/bridge/bridge.hpp"

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
    picker.draw({0,0}, Picker::DRAW_PICKER);
    picker.setImage(framebufferObject()->toImage(false));

    //  toImage releases binding for some reason
    framebufferObject()->bind();
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // Draw all of the scene objects
    blitter.draw(M);
    axes.draw(M);
    picker.draw(mouse.toPoint());
}

void Canvas::push(int instance_index, const QString& instance_name,
                  const QString& sheet_name)
{
    (void)instance_name;
    (void)sheet_name;

    if (env.size() == 0)
    {
        assert(visited.size() == 0);
        picker.beginUpdate();
    }
    env.push_back(instance_index);
}

void Canvas::pop()
{
    env.pop_back();

    if (env.size() == 0)
    {
        std::set<ShapeKey> to_erase;
        for (auto s : shapes)
        {
            if (visited.find(s.first) == visited.end())
            {
                s.second->deleteWhenNotRunning();
                to_erase.insert(s.first);
            }
        }
        for (auto e : to_erase)
        {
            shapes.erase(e);
        }
        visited.clear();

        if (picker.endUpdate())
        {
            update();
        }
    }
}

void Canvas::cell(int c, const QString& name, const QString& expr, int type,
                  bool valid, const QString& val, void* ptr)
{
    (void)name;
    (void)expr;
    (void)type;
    (void)val;

    Graph::CellKey key = {env, c};
    if (valid)
    {
        auto v = Graph::Interpreter::untag(static_cast<Graph::ValuePtr>(ptr));

        // If this is a (Scheme) shape, then extract it and make a Renderer
        // for it (if one doesn't already exist)
        if (Kernel::Bind::is_shape(v))
        {
            installShape({key, Kernel::Bind::get_shape(v)});
        }
        else if (picker.isHandle(v) && picker.installHandle(key, v))
        {
            update();
        }
    }
}

void Canvas::installShape(const ShapeKey& k)
{
    if (shapes.count(k) == 0)
    {
        auto r = new App::Render::Renderer(k.second->tree);
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
    else
    {
        auto r = shapes.at(k);
        if (r->updateVars(k.second->tree))
        {
            r->onViewChanged(M, window_size);
        }
    }

    // Make sure we don't delete this renderer
    visited.insert(k);
}

void Canvas::synchronize(QQuickFramebufferObject *item)
{
    auto c = dynamic_cast<CanvasObject*>(item);
    assert(c);

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

////////////////////////////////////////////////////////////////////////////////

QQuickFramebufferObject::Renderer* CanvasObject::createRenderer() const
{
    auto c = new Canvas();
    return c;
}

void CanvasObject::rotateIncremental(float dx, float dy)
{
    pitch += dy;
    yaw += dx;

    pitch = fmax(fmin(pitch, 180), 0);
    yaw = fmod(yaw, 360);
    update();
}

void CanvasObject::panIncremental(float dx, float dy)
{
    // Find the starting position in world coordinates
    auto inv = M().inverted();
    auto diff = inv.map({float(dx/width()), float(dy/height()), 0}) -
                inv.map({0, 0, 0});

    center += diff*2;
    update();
}

void CanvasObject::zoomIncremental(float ds, float x, float y)
{
    QVector3D pt(x / width() - 0.5, y / height() - 0.5, 0);
    auto a = M().inverted().map(pt);

    scale *= pow(1.1, ds / 120.);
    center += 2 * (M().inverted().map(pt) - a);
    update();
}

void CanvasObject::mouseMove(float x, float y)
{
    if (mouse_state == DRAG_ROT)
    {
        rotateIncremental(x - mouse_pos.x(), y - mouse_pos.y());
    }
    else if (mouse_state == DRAG_PAN)
    {
        panIncremental(x - mouse_pos.x(), y - mouse_pos.y());
    }
    else if (mouse_state == DRAG_HANDLE)
    {
        QVector2D mouse_gl(
            2 * mouse_pos.x() / width() - 1,
            2 * mouse_pos.y() / height() - 1);
        assert(mouse_drag != nullptr);
        auto sol = mouse_drag->dragTo(M().inverted(), mouse_gl);
        App::Core::Bridge::instance()->setVariables(sol);
    }
    else
    {
        update();
    }
    mouse_pos = {x, y};
}

void CanvasObject::mouseClick(int button)
{
    if (mouse_state == RELEASED)
    {
        if (button == Qt::LeftButton)
        {
            mouse_state = CLICK_LEFT;
            update();
        }
        else if (button == Qt::RightButton)
        {
            mouse_state = CLICK_RIGHT;
            update();
        }
    }
}

void CanvasObject::mouseRelease()
{
    if (mouse_state == DRAG_HANDLE)
    {
        mouse_drag = nullptr;
    }
    mouse_state = RELEASED;
    update();
}

QMatrix4x4 CanvasObject::proj() const
{
    QMatrix4x4 m;

    //  Compress the Z axis to avoid clipping
    //  The value 4 isn't anything magical here, just seems to work well
    const float Z_COMPRESS = 4;
    const float frac = width() / float(height());
    if (frac > 1)
    {
        m.scale(1/frac, 1, 1/Z_COMPRESS);
    }
    else
    {
        m.scale(1, frac, 1/Z_COMPRESS);
    }
    return m;
}

QMatrix4x4 CanvasObject::view() const
{
    QMatrix4x4 m;
    m.scale(scale, scale, scale);
    m.rotate(pitch, {1, 0, 0});
    m.rotate(yaw,   {0, 0, 1});
    m.translate(center);

    return m;
}

}   // namespace Render
}   // namespace App
