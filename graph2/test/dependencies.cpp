#include "catch/catch.hpp"

#include "graph/dependencies.hpp"
#include "graph/root.hpp"

TEST_CASE("Dependencies::insert")
{
    Root r;
    Dependencies deps(r);

    REQUIRE(!deps.insert({{{0}}, {0}}, {{{0}}, "a"}));
}
