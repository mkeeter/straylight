#include <catch/catch.hpp>

#include "kernel/tree/tree.hpp"

#include "kernel/format/contours.hpp"
#include "kernel/render/region.hpp"

// Overloaded toString for glm::vec3
#include "util/glm.hpp"
#include "util/shapes.hpp"

using namespace Kernel;

TEST_CASE("Contours::render (adjacent rectangles)")
{
    auto rects = min(rectangle(-1, 0, -1, 1), rectangle(0, 1, -1, 1));
    Region r({-2, 2}, {-2, 2}, {0, 0}, 2);

    auto cs_pos = Contours::render(rects, r);
    REQUIRE(cs_pos.contours.size() == 1);

    auto cs_neg = Contours::render(-rects, r);
    REQUIRE(cs_neg.contours.size() == 1);
}
