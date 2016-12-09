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
    auto a = s.insertInstance("a", 0);

    REQUIRE(a.i == 0);
    REQUIRE(!s.canInsert("a"));
}

TEST_CASE("Sheet::rename")
{
    Sheet s;
    auto a = s.insertInstance("a", 0);
    auto b = s.insertCell("b", "(+ 1 2)");

    s.rename(a, "c");
    s.rename(b, "d");

    REQUIRE(!s.canInsert("c"));
    REQUIRE(!s.canInsert("d"));
}

TEST_CASE("Sheet::at(ItemIndex)")
{
    Sheet s;
    auto a = s.insertInstance("a", 0);
    auto b = s.insertCell("b", "(+ 1 2)");

    s.at(a);
    s.at(b);
    REQUIRE(true); // Didn't crash!

    bool threw = false;
    try
    {
        auto c = s.at(2);
    }
    catch (const std::out_of_range&)
    {
        threw = true;
    }
    REQUIRE(threw);
}

TEST_CASE("Sheet::at(std::string)")
{
    Sheet s;
    auto a = s.insertInstance("a", 0);
    auto b = s.insertCell("b", "(+ 1 2)");

    s.at("a");
    s.at("b");
    REQUIRE(true); // Didn't crash!

    bool threw = false;
    try
    {
        auto c = s.at("c");
    }
    catch (const std::out_of_range&)
    {
        threw = true;
    }
    REQUIRE(threw);
}

TEST_CASE("Sheet::hasItem")
{
    Sheet s;
    auto a = s.insertInstance("a", 0);
    auto b = s.insertCell("b", "(+ 1 2)");

    REQUIRE(s.hasItem("a"));
    REQUIRE(s.hasItem("b"));
    REQUIRE(!s.hasItem("c"));
}

TEST_CASE("Sheet::nameOf")
{
    Sheet s;
    auto a = s.insertInstance("a", 0);
    auto b = s.insertCell("b", "(+ 1 2)");

    REQUIRE(s.nameOf(a) == "a");
    REQUIRE(s.nameOf(b) == "b");
}
