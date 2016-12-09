#include "catch/catch.hpp"

#include "graph/dependencies.hpp"
#include "graph/root.hpp"

TEST_CASE("Dependencies::insert")
{
    Root r;
    Dependencies deps(r);

    REQUIRE(!deps.insert({{0}, 0}, {{0}, "a"}));
    REQUIRE(!deps.insert({{1}, 0}, {{0}, "a"}));
    REQUIRE(!deps.insert({{1}, 0}, {{1}, "a"}));

    // TODO: test this more once we can create circular loops
    // in Root
}
