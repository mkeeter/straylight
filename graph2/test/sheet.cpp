#include "catch/catch.hpp"

#include "graph/sheet.hpp"

TEST_CASE("Sheet::canInsert")
{
    Sheet s;

    REQUIRE(s.canInsert("a"));
    REQUIRE(s.canInsert("b"));
    REQUIRE(!s.canInsert(""));
}

TEST_CASE("Sheet::insertCell")
{
    Sheet s;
    auto a = s.insertCell("a", "(+ 1 2)");

    REQUIRE(a.i == 0);
    REQUIRE(!s.canInsert("a"));
}

TEST_CASE("Sheet::insertInstance")
{
    Sheet s;
    auto a = s.insertInstance("a", {0});

    REQUIRE(a.i == 0);
    REQUIRE(!s.canInsert("a"));
}

TEST_CASE("Sheet::rename")
{
    Sheet s;
    auto a = s.insertInstance("a", {0});
    auto b = s.insertCell("a", "(+ 1 2)");

    s.rename(a, "c");
    s.rename(b, "d");

    REQUIRE(!s.canInsert("c"));
    REQUIRE(!s.canInsert("d"));
}
