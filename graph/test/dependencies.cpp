#include "catch/catch.hpp"

#include "graph/dependencies.hpp"
#include "graph/root.hpp"

using namespace Graph;

TEST_CASE("Dependencies::insert")
{
    Root r;
    Dependencies deps(r);

    REQUIRE(!deps.insert({{Tree::ROOT_INSTANCE}, 0},
                         {{Tree::ROOT_INSTANCE}, "a"}));

    // TODO: test this more once we can create circular loops in Root
}

TEST_CASE("Dependencies::clear")
{
    // TODO
}
