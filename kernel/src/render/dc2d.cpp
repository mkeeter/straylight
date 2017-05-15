#include <set>
#include <map>

#include <glm/gtx/common.hpp>

#include "kernel/format/contours.hpp"
#include "kernel/render/region.hpp"

namespace Kernel {

struct Comparator
{
    bool operator()(const glm::vec2& a, const glm::vec2& b) const
    {
        if (a.x != b.x)
        {
            return a.x < b.x;
        }
        else
        {
            return a.y < b.y;
        }
    }
};

/*
 *  Helper struct that can be passed around when making contours
 */
namespace DC
{

struct Worker2D
{
    Worker2D() {}

    /*
     *  Mutually recursive functions to get a mesh from an Octree
     */
    void cell(const Quadtree* c);
    void edge(const Quadtree* a, const Quadtree* b, Axis axis);

    /*
     *  Write out the given segment into the segment list
     */
    void segment(const Quadtree* a, const Quadtree* b);

    std::list<std::pair<glm::vec2, glm::vec2>> segments;
};

void Worker2D::cell(const Quadtree* c)
{
    if (c->getType() == Quadtree::BRANCH)
    {
        // Recurse down every subface in the quadtree
        for (int i=0; i < 4; ++i)
        {
            cell(c->child(i));
        }

        //  Then, call edge on every pair of cells
        edge(c->child(0), c->child(AXIS_X), AXIS_X);
        edge(c->child(AXIS_Y),
             c->child(AXIS_Y | AXIS_X),
             AXIS_X);
        edge(c->child(0), c->child(AXIS_Y), AXIS_Y);
        edge(c->child(AXIS_X),
             c->child(AXIS_X | AXIS_Y),
             AXIS_Y);
    }
}

void Worker2D::edge(const Quadtree* a, const Quadtree* b, Axis axis)
{
    if (a->getType() == Quadtree::LEAF && b->getType() == Quadtree::LEAF)
    {
        bool crossed;
        bool ordering;

        //  See detailed comment in the 3D implementation
        if (a->getLevel() < b->getLevel())
        {
            crossed = a->corner(axis) != a->corner(3);
            ordering = a->corner(axis);
        }
        else
        {
            crossed = b->corner(0) != b->corner(3 ^ axis);
            ordering = b->corner(0);
        }

        if (crossed)
        {
            if (ordering ^ (axis == AXIS_X))
            {
                segment(a, b);
            }
            else
            {
                segment(b, a);
            }
        }
    }
    else if (a->getType() == Quadtree::BRANCH || b->getType() == Quadtree::BRANCH)
    {
        edge(a->child(axis), b->child(0), axis);
        edge(a->child(3), b->child(3 ^ axis), axis);
    }
}

void Worker2D::segment(const Quadtree* a, const Quadtree* b)
{
    auto va = a->getVertex();
    auto vb = b->getVertex();

    segments.push_back({{va.x, va.y}, {vb.x, vb.y}});
}
}

////////////////////////////////////////////////////////////////////////////////

Contours Contours::render(const Tree t, const Region& r)
{
    std::unique_ptr<Quadtree> q(Quadtree::render(t, r));

    DC::Worker2D w;
    w.cell(q.get());

    std::map<glm::vec2, glm::vec2, Comparator> segs;
    for (const auto& s : w.segments)
    {
        if (s.first != s.second)
        {
            segs[s.first] = s.second;
        }
    }

    Contours c;
    while (segs.size())
    {
        auto front = *segs.begin();
        std::vector<glm::vec2> vec = {front.first};
        std::set<glm::vec2, Comparator> found;

        auto back = front.first;
        // Walk around the contour until we hit a vertex that we've already
        // seen or we run out of vertices to walk around
        while (!found.count(back))
        {
            found.insert(back);
            auto b = segs.find(back);
            if (b == segs.end())
            {
                break;
            }
            else
            {
                back = b->second;
                segs.erase(b);
            }

            vec.push_back(back);
        }
        c.contours.emplace_back(std::move(vec));
    }
    return c;
}

}   // namespace Kernel
