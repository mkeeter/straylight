#include "catch/catch.hpp"

#include "graph/tree.hpp"

TEST_CASE("Tree::canInsert")
{
    Tree s;

    REQUIRE(s.canInsert("a", 0));
    REQUIRE(s.canInsert("b", 0));
    REQUIRE(!s.canInsert("", 0));
}

TEST_CASE("Tree::insertCell")
{
    Tree s;
    auto a = s.insertCell("a", "(+ 1 2)", 0);

    REQUIRE(a.i == 0);
    REQUIRE(!s.canInsert("a", 0));
}

TEST_CASE("Tree::insertInstance")
{
    Tree s;
    auto a = s.insertInstance("a", 0, 0);

    REQUIRE(a.i == 0);
    REQUIRE(!s.canInsert("a", 0));
}

TEST_CASE("Tree::rename")
{
    Tree s;
    auto a = s.insertInstance("a", 0, 0);
    auto b = s.insertCell("b", "(+ 1 2)", 0);

    s.rename(a, "c");
    s.rename(b, "d");

    REQUIRE(!s.canInsert("c", 0));
    REQUIRE(!s.canInsert("d", 0));
}

TEST_CASE("Tree::at(ItemIndex)")
{
    Tree s;
    auto a = s.insertInstance("a", 0, 0);
    auto b = s.insertCell("b", "(+ 1 2)", 0);

    s.at(a);
    s.at(b);
    REQUIRE(true); // Didn't crash!

    bool threw = false;
    try
    {
        auto c = s.at(3);
    }
    catch (const std::out_of_range&)
    {
        threw = true;
    }
    REQUIRE(threw);
}

TEST_CASE("Tree::at(std::string)")
{
    Tree s;
    auto a = s.insertInstance("a", 0, 0);
    auto b = s.insertCell("b", "(+ 1 2)", 0);

    s.at("a", 0);
    s.at("b", 0);
    REQUIRE(true); // Didn't crash!

    bool threw = false;
    try
    {
        auto c = s.at("c", 0);
    }
    catch (const std::out_of_range&)
    {
        threw = true;
    }
    REQUIRE(threw);
}

TEST_CASE("Tree::hasItem")
{
    Tree s;
    auto a = s.insertInstance("a", 0, 0);
    auto b = s.insertCell("b", "(+ 1 2)", 0);

    REQUIRE(s.hasItem("a", 0));
    REQUIRE(s.hasItem("b", 0));
    REQUIRE(!s.hasItem("c", 0));
}

TEST_CASE("Tree::nameOf")
{
    Tree s;
    auto a = s.insertInstance("a", 0, 0);
    auto b = s.insertCell("b", "(+ 1 2)", 0);

    REQUIRE(s.nameOf(a) == "a");
    REQUIRE(s.nameOf(b) == "b");
}

