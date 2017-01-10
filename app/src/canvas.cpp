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
    float width = window_size.width();
    float height = window_size.height();

    QMatrix4x4 m;

    if (width > height)
    {
        const float frac = height/float(width);
        m.scale(frac, -1.0, -frac);
    }
    else
    {
        const float frac = width/float(height);
        m.scale(1.0, -frac, -frac);
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
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    const auto mat = M();
    axes.draw(mat);
    blitter.draw(mat);
}

void Canvas::push(Graph::InstanceIndex i)
{
    if (env.size() == 0)
    {
        assert(visited.size() == 0);
    }
    env.push_back(i);
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
    }
}

void Canvas::cell(Graph::CellIndex c, const Graph::Root* r)
{
    assert(r);

    Graph::CellKey key = {env, c};
    if (r->isValid(key))
    {
        auto v = r->getRawValuePtr(key);

        // If this is a (Scheme) shape, then extract it and make a Renderer
        // for it (if one doesn't already exist)
        if (is_shape(v))
        {
            auto s = get_shape(v);
            if (shapes.count({key, s}) == 0)
            {
                auto r = new ::Renderer(s->tree);
                shapes[{key, s}] = r;

                // Connect the renderer to our viewChanged signal so it will
                // automatically redraw when necessary
                connect(this, &Canvas::viewChanged,
                        r, &::Renderer::onViewChanged);

                // Connect the renderer and the blitter, so bitmaps will
                // magically appear in the 3D viewport
                connect(r, &::Renderer::gotResult,
                        &blitter, &Blitter::addQuad);
                connect(r, &::Renderer::goodbye,
                        &blitter, &Blitter::forget);

                // Kick off an initial render
                r->onViewChanged(M(), window_size);
            }

            // Make sure we don't delete this renderer
            visited.insert({key, s});
        }
    }
}

void Canvas::setSize(float w, float h)
{
    window_size = QSize(w, h);
    emit(viewChanged(M(), window_size));
    update();
}

void Canvas::rotateIncremental(float dx, float dy)
{
    pitch += dy;
    yaw += dx;

    pitch = fmin(fmax(pitch, -180), 0);
    yaw = fmod(yaw, 360);

    emit(viewChanged(M(), window_size));
    update();
}

void Canvas::panIncremental(float dx, float dy)
{
    // Find the starting position in world coordinates
    auto inv = M().inverted();
    auto diff = inv.map({dx/window_size.width(), dy/window_size.height(), 0}) -
                inv.map({0, 0, 0});

    center += diff*2;
    emit(viewChanged(M(), window_size));
    update();
}

void Canvas::zoomIncremental(float ds, float x, float y)
{
    QVector3D pt(x / window_size.width() - 0.5,
                 y / window_size.height() - 0.5, 0);
    auto a = M().inverted().map(pt);

    scale *= pow(1.1, ds / 120.);
    center += 2 * (M().inverted().map(pt) - a);

    emit(viewChanged(M(), window_size));
    update();
}

////////////////////////////////////////////////////////////////////////////////

QQuickFramebufferObject::Renderer* CanvasObject::createRenderer() const
{
    auto c = new Canvas();
    Bridge::singleton()->setCanvas(c);
    return c;
}
