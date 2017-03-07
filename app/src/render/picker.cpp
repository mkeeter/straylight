#include <boost/algorithm/string/predicate.hpp>

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
        return (f == colors.left.end()) ? nullptr : f->second;
    }
}

void Picker::draw(QPoint p, const Graph::Env& env, Picker::DrawMode mode)
{
    auto picked = pickAt(p);

    //  Store a list of handles for each rank, so that we draw in
    //  correct order (e.g. points above shapes)
    std::map<int, std::list<Handle*>> to_draw;

    for (auto& h : handles)
    {
        if (boost::algorithm::starts_with(env, h.first.first))
        {
            to_draw[h.second->tag()].push_back(h.second);
        }
    }

    for (const auto& level : to_draw)
    {
        for (auto& h : level.second)
        {
            h->draw(M, proj, (mode == DRAW_NORMAL)
                    ? (h == picked ? DRAW_HOVER : DRAW_NORMAL)
                    : mode, colors.right.at(h));
        }
    }
}

void Picker::installHandle(const Graph::CellKey& k, Handle* h)
{
    if (handles.find(k) == handles.end())
    {
        handles.insert({k, h});
        auto rgb = colors.size() ? (colors.left.rbegin()->first + 1) : 1;
        colors.insert({rgb, h});
    }
}

void Picker::prune(const std::set<Graph::CellKey>& hs)
{
    std::set<Graph::CellKey> forgotten;
    for (auto h : handles)
    {
        if (hs.find(h.first) == hs.end())
        {
            delete h.second;
            forgotten.insert(h.first);
        }
    }

    for (auto h : forgotten)
    {
        colors.right.erase(handles.at(h));
        handles.erase(h);
    }
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

Handle* Picker::getHandle(const Graph::CellKey& k) const
{
    return handles.count(k) ? handles.at(k) : nullptr;
}

}   // namespace Render
}   // namespace App
