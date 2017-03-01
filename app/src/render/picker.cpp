#include "app/render/picker.hpp"
#include "app/render/point_handle.hpp"

#include "app/bind/bind_s7.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace App {
namespace Render {

Picker::Picker()
{
    initializeOpenGLFunctions();
}

Handle* Picker::pickAt(QPoint p)
{
    if (p.x() >= img.size().width() ||
        p.y() >= img.size().height() ||
        p.x() < 0 || p.y() < 0 ||
        img.isNull())
    {
        return nullptr;
    }
    else
    {
        auto rgb = img.pixel(p) & 0xFFFFFF; // Mask alpha bits

        auto f = colors.left.find(rgb);
        return (f == colors.left.end()) ? nullptr : handles.at(f->second);
    }
}

void Picker::draw(QPoint p, Picker::DrawMode mode)
{
    auto picked = pickAt(p);
    for (auto& h : handles)
    {
        h.second->draw(M, proj, (mode == DRAW_NORMAL)
                ? (h.second == picked ? DRAW_HOVER : DRAW_NORMAL)
                : mode, colors.right.at(h.first));
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

bool Picker::installHandle(const Graph::CellKey& k, Graph::ValuePtr p)
{
    bool changed = false;

    // TODO: once we have more than one handle type,
    // check whether the types match here
    auto tag = App::Bind::get_handle_tag(p);
    HandleKey key = {k, tag};
    if (handles.count(key) == 0)
    {
        handles[key] = new PointHandle(App::Bind::get_point_handle(p)->drag);
        auto rgb = colors.size() ? (colors.left.rbegin()->first + 1) : 1;
        colors.insert({rgb, key});
        changed = true;
    }
    visited.insert(key);

    changed |= handles[key]->updateFrom(p);
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

void Picker::setView(QMatrix4x4 mat, QSize size)
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
