#include <catch/catch.hpp>

#include "kernel/tree/tree.hpp"

#include "kernel/render/quadtree.hpp"
#include "kernel/render/region.hpp"

// Overloaded toString for glm::vec3
#include "util/glm.hpp"
#include "util/shapes.hpp"

using namespace Kernel;

TEST_CASE("Quadtree: coordinates")
{
    Tree t = circle(1);

    Region r({-1, 1}, {-1, 1}, {0, 0}, 1);
    std::unique_ptr<Quadtree> out(Quadtree::render(t, r));
    REQUIRE(out->getType() == Quadtree::BRANCH);

    // Check that all child pointers are populated
    for (int i=0; i < 4; ++i)
    {
        CAPTURE(i);
        CAPTURE(out->child(i));
        REQUIRE(out->child(i) != nullptr);
    }

    // Check that Subregion::octsect and Quadtree::pos have consistent ordering
    for (int i=0; i < 4; ++i)
    {
        REQUIRE(out->pos(i) == out->child(i)->pos(i));
    }
}

TEST_CASE("Quadtree: vertex positioning")
{
    const float radius = 0.5;
    Tree t = circle(radius);

    Region r({-1, 1}, {-1, 1}, {0, 0}, 4);

    std::unique_ptr<Quadtree> out(Quadtree::render(t, r));

    // Walk every leaf node in the octree, keeping track of the
    // minimum and maximum vertex radius
    float rmax = -std::numeric_limits<float>::infinity();
    float rmin =  std::numeric_limits<float>::infinity();

    // Queue of octrees to process
    std::list<const Quadtree*> targets = {out.get()};
    while (targets.size())
    {
        const Quadtree* o = targets.front();
        targets.pop_front();

        if (o->getType() == Quadtree::BRANCH)
        {
            for (unsigned i=0; i < 4; ++i)
            {
                targets.push_back(o->child(i));
            }
        }
        else if (o->getType() == Quadtree::LEAF)
        {
            float r = glm::length(o->getVertex());
            rmax = std::max(r, rmax);
            rmin = std::max(r, rmin);
        }
    }

    REQUIRE(rmin > radius*0.9);
    REQUIRE(rmax < radius*1.1);
}
