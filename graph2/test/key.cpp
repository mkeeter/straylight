#include "catch/catch.hpp"

#include "graph/dependencies.hpp"

using namespace Graph;

TEST_CASE("CellKey::comparisons")
{
    CellKey a = {0,{}};
    CellKey b = {0,{1}};
    CellKey c = {0,{1,2}};
    CellKey d = {0,{1,2,3}};
    CellKey e = {0,{2}};
    CellKey f = {1,{}};

    REQUIRE(a < b);
    REQUIRE(b < c);
    REQUIRE(c < d);
    REQUIRE(d < e);
    REQUIRE(e < f);

    REQUIRE(!(b < a));
    REQUIRE(!(c < b));
    REQUIRE(!(d < c));
    REQUIRE(!(e < d));
    REQUIRE(!(f < e));
}

TEST_CASE("CellKey::specializes")
{
    CellKey a = {0,{}};
    CellKey b = {0,{1}};
    CellKey c = {0,{1,2}};
    CellKey d = {0,{1,2,3}};
    CellKey e = {0,{2}};
    CellKey f = {1,{}};

    REQUIRE(b.specializes(a));
    REQUIRE(!a.specializes(b));
    REQUIRE(c.specializes(a));
    REQUIRE(c.specializes(b));
    REQUIRE(!c.specializes(d));
    REQUIRE(!c.specializes(e));
    REQUIRE(!c.specializes(f));
}
