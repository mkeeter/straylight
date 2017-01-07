#include <QOpenGLFramebufferObject>

#include "canvas.hpp"
#include "bridge.hpp"

#include "kernel/bind/bind_s7.h"

QOpenGLFramebufferObject* Canvas::createFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);

    return new QOpenGLFramebufferObject(size, format);
}

QMatrix4x4 Canvas::proj() const
{
    float width = window_size.x();
    float height = window_size.y();

    QMatrix4x4 m;

    if (width > height)
    {
        const float frac = height/float(width);
        m.scale(frac, 1.0, -frac);
    }
    else
    {
        const float frac = width/float(height);
        m.scale(1.0, frac, -frac);
    }
    return m;
}

QMatrix4x4 Canvas::view() const
{
    QMatrix4x4 m;
    m.scale(scale, scale, scale);
    m.rotate(pitch, {1, 0, 0});
    m.rotate(yaw,   {0, 0, 1});
    m.translate(center);

    return m;
}

void Canvas::render()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    axes.draw(M());

    // Do rendering here
}

void Canvas::push(Graph::InstanceIndex i)
{
    env.push_back(i);
}

void Canvas::pop()
{
    env.pop_back();
}

void Canvas::cell(Graph::CellIndex c, const Graph::Root* r)
{
    assert(r);

    Graph::CellKey key = {env, c};
    if (r->isValid(key))
    {
        auto v = r->getRawValuePtr(key);

        // Low-level functions to strip the Shape from the data allocated by
        // the interpreter
        if (is_shape(v))
        {
            auto s = get_shape(v);
        }
    }
}

void Canvas::setSize(float w, float h)
{
    window_size = {w, h};
    update();
}

void Canvas::rotateIncremental(float dx, float dy)
{
    pitch -= dy;
    yaw -= dx;

    pitch = fmin(fmax(pitch, 0), 180);
    yaw = fmod(yaw, 360);

    update();
}

void Canvas::panIncremental(float dx, float dy)
{
    // Find the starting position in world coordinates
    auto inv = M().inverted();
    auto diff = inv.map({dx/window_size.x(), dy/window_size.y(), 0}) - 
                inv.map({0, 0, 0});

    center += diff*2;
    update();
}

////////////////////////////////////////////////////////////////////////////////

QQuickFramebufferObject::Renderer* CanvasObject::createRenderer() const
{
    auto c = new Canvas();
    Bridge::singleton()->setCanvas(c);
    return c;
}
