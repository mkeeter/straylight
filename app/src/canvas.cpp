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
    /*
    glm::mat4 m = glm::scale(glm::vec3(scale, scale, scale));;
    m = glm::rotate(m, pitch,  {1.0, 0.0, 0.0});
    m = glm::rotate(m, roll, {0.0, 0.0, 1.0});
    m = glm::translate(m, center);

    return m;
    */
    return QMatrix4x4();
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
            printf("Got shape %p\n", s);
        }
    }
}

void Canvas::setSize(float w, float h)
{
    window_size = {w, h};
    update();
}

////////////////////////////////////////////////////////////////////////////////

QQuickFramebufferObject::Renderer* CanvasObject::createRenderer() const
{
    auto c = new Canvas();
    Bridge::singleton()->setCanvas(c);
    return c;
}
