#include "catch/catch.hpp"

#include "graph/item.hpp"

using namespace Graph;

TEST_CASE("Item::instance")
{
    auto i = Item((SheetIndex){0});
    REQUIRE(i.instance() != nullptr);
    REQUIRE(i.cell() == nullptr);
    i.dealloc();
}

TEST_CASE("Item::cell")
{
    auto i = Item("(+ 1 2)");
    REQUIRE(i.instance() == nullptr);
    REQUIRE(i.cell() != nullptr);
    i.dealloc();
}
