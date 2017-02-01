#include <QOpenGLFramebufferObject>

#include "canvas.hpp"
#include "bridge.hpp"

#include "kernel/bind/bind_s7.h"

Canvas::Canvas()
{
    connect(&blitter, &Blitter::changed, [=](){ this->update(); });
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

    const auto mat = M;
    axes.draw(mat);
    blitter.draw(mat);
}

void Canvas::push(int instance_index, const QString& instance_name,
                  const QString& sheet_name)
{
    (void)instance_name;
    (void)sheet_name;

    if (env.size() == 0)
    {
        assert(visited.size() == 0);
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
                        &blitter, &Blitter::addQuad, Qt::DirectConnection);
                connect(r, &::Renderer::goodbye,
                        &blitter, &Blitter::forget);

                // Kick off an initial render
                r->onViewChanged(M, window_size);
            }

            // Make sure we don't delete this renderer
            visited.insert({key, s});
        }
    }
}

void Canvas::synchronize(QQuickFramebufferObject *item)
{
    auto c = dynamic_cast<CanvasObject*>(item);
    assert(c);

    const auto M_ = c->M();
    const QSize window_size_(c->width(), c->height());
    const bool changed = (M != M_) || (window_size != window_size_);

    if (changed)
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
    Bridge::singleton()->attachCanvas(c);
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

QMatrix4x4 CanvasObject::proj() const
{
    QMatrix4x4 m;

    //  Compress the Z axis to avoid clipping
    //  The value 4 isn't anything magical here, just seems to work well
    const float Z_COMPRESS = 4;
    const float frac = width() / height();
    if (frac > 1)
    {
        m.scale(1/frac, 1, 1/Z_COMPRESS);
    }
    else
    {
        const float frac = width()/float(height());
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
