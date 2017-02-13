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

    auto f = colors.left.find(rgb);
    return (f == colors.left.end()) ? nullptr : handles.at(f->second);
}

void Picker::draw(QPoint p)
{
    (void)p;
    //auto picked = pickAt(p);
    for (auto& h : handles)
    {
        h.second->draw(M, proj, Handle::BASE);
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

bool Picker::installHandle(const HandleKey& k)
{
    bool changed = false;

    // TODO: once we have more than one handle type,
    // check whether the types match here
    if (handles.count(k) == 0)
    {
        handles[k] = new PointHandle();
        auto rgb = colors.size() ? (colors.left.rbegin()->first + 1) : 1;
        colors.insert({rgb, k});
        changed = true;
    }
    visited.insert(k);

    changed |= handles[k]->updateFrom(k.second);
    return changed;
}

bool Picker::endUpdate()
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
        colors.right.erase(e);
    }

    return to_erase.size();
}

void Picker::onViewChanged(QMatrix4x4 mat, QSize size)
{
    M = mat;
    window_size = size;

    //  Compress the Z axis to avoid clipping
    //  The value 4 isn't anything magical here, just seems to work well
    const float frac = window_size.width() / float(window_size.height());
    proj = QMatrix4x4();
    if (frac > 1)
    {
        proj.scale(1/frac, 1, 1);
    }
    else
    {
        proj.scale(1, frac, 1);
    }
}

}   // namespace Render
}   // namespace App