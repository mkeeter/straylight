#include "render/picker.hpp"
#include "render/point_handle.hpp"

#include "bind/bind_s7.h"

////////////////////////////////////////////////////////////////////////////////

namespace App {
namespace Render {

Picker::Picker()
{
    initializeOpenGLFunctions();
}

Handle* Picker::pickAt(QPoint p)
{
    auto rgb = img.pixel(p);

    auto f = colors.find(rgb);
    return (f == colors.end()) ? nullptr : f->second;
}

void Picker::draw(QPoint p)
{
    auto picked = pickAt(p);
    for (auto& h : handles)
    {
        h.second->draw(M, h.second == picked);
    }
}

void Picker::beginUpdate()
{
    visited.clear();
}

bool Picker::isHandle(Graph::ValuePtr ptr)
{
    return App::Bind::is_point_handle(ptr);
}

void Picker::installHandle(const HandleKey& k)
{
    // TODO: once we have more than one handle type,
    // check whether the types match here
    if (handles.count(k) == 0)
    {
        handles[k] = new PointHandle();
    }
    handles[k]->updateFrom(k.second);

    visited.insert(k);
}

void Picker::endUpdate()
{
    std::set<HandleKey> to_erase;
    for (auto h : handles)
    {
        if (visited.find(h.first) == visited.end())
        {
            to_erase.insert(h.first);
        }
    }
    for (auto e : to_erase)
    {
        delete handles[e];
        handles.erase(e);
    }
}

void Picker::onViewChanged(QMatrix4x4 mat, QSize size)
{
    bool changed = (M != mat) || (window_size != size);

    M = mat;
    window_size = size;

    (void)changed;
}

}   // namespace Render
}   // namespace App
