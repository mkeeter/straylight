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

void Canvas::render()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    axes.draw(QMatrix4x4());

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

////////////////////////////////////////////////////////////////////////////////

QQuickFramebufferObject::Renderer* CanvasObject::createRenderer() const
{
    auto c = new Canvas();
    Bridge::singleton()->setCanvas(c);
    return c;
}
