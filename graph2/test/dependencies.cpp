#include "catch/catch.hpp"

#include "graph/dependencies.hpp"
#include "graph/root.hpp"

TEST_CASE("Dependencies::insert")
{
    Root r;
    Dependencies deps(r);

    REQUIRE(!deps.insert({{}, 0}, {{}, "a"}));

    // TODO: test this more once we can create circular loops in Root
}

TEST_CASE("Dependencies::clear")
{
    // TODO
}

TEST_CASE("Dependencies::clearAll")
{
    // TODO
}
