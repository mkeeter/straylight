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

void Picker::draw(QPoint p, Picker::DrawMode mode)
{
    auto picked = pickAt(p);
    for (auto& h : handles)
    {
        h->draw(M, proj, (mode == DRAW_NORMAL)
                ? (h == picked ? DRAW_HOVER : DRAW_NORMAL)
                : mode, colors.right.at(h));
    }
}

void Picker::installHandle(Handle* h)
{
    if (handles.find(h) == handles.end())
    {
        handles.insert(h);
        auto rgb = colors.size() ? (colors.left.rbegin()->first + 1) : 1;
        colors.insert({rgb, h});
    }
}

void Picker::prune(const QSet<Handle*>& hs)
{
    QSet<Handle*> forgotten;
    for (auto h : handles)
    {
        if (hs.find(h) == hs.end())
        {
            delete h;
            forgotten.insert(h);
        }
    }

    for (auto h : forgotten)
    {
        handles.erase(h);
        colors.right.erase(h);
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

}   // namespace Render
}   // namespace App
