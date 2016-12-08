#include "catch/catch.hpp"

#include "graph/sheet.hpp"

TEST_CASE("Sheet::canInsert")
{
    Sheet s;

    REQUIRE(s.canInsert("a"));
    REQUIRE(s.canInsert("b"));
    REQUIRE(!s.canInsert(""));

    s.insertCell("a", "(+ 1 2)");
    REQUIRE(!s.canInsert("a"));
}
