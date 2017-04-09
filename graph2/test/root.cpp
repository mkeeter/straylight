#include "catch/catch.hpp"

#include "graph/root.hpp"
#include "graph/sheet.hpp"

using namespace Graph;

TEST_CASE("Root::insert(Sheet)")
{
    Root r;
    auto s = r.sheets.insert(new Sheet);

    auto i = r.insert("omg", new Sheet(s));
    REQUIRE(i.i == 5);
    REQUIRE(r.sheets.at(s)->sheets.left.find("omg") !=
            r.sheets.at(s)->sheets.left.end());
}
