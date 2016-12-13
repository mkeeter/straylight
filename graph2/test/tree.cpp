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

    REQUIRE(a.i == 1);
    REQUIRE(!s.canInsert("a", 0));
}

TEST_CASE("Tree::insertInstance")
{
    Tree s;
    auto a = s.insertInstance("a", 0, 0);

    REQUIRE(a.i == 1);
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
        auto c = s.at(99);
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

TEST_CASE("Tree::iterItems")
{
    Tree t;

    SECTION("Top level")
    {
        auto a = t.insertInstance("a", 0, 0);
        auto b = t.insertCell("b", "(+ 1 2)", 0);
        auto iter = t.iterItems(0);
        REQUIRE(iter.size() == 2);
        REQUIRE(iter.front() == 1); // a
        iter.pop_front();
        REQUIRE(iter.front() == 2); // b

        auto i1 = t.iterItems(1);
        REQUIRE(i1.size() == 0);
    }

    SECTION("Nested")
    {
        auto a = t.insertInstance("a", 0, 1);
        auto b = t.insertCell("b", "(+ 1 2)", 2);

        auto i0 = t.iterItems(0);
        REQUIRE(i0.size() == 0);

        auto i1 = t.iterItems(1);
        REQUIRE(i1.size() == 1);
        REQUIRE(i1.front() == 1); // a

        auto i2 = t.iterItems(2);
        REQUIRE(i2.size() == 1);
        REQUIRE(i2.front() == 2); // b
    }
}

TEST_CASE("Tree::parentOf")
{
    Tree t;

    auto a = t.insertInstance("a", 0, 1);
    auto b = t.insertCell("b", "(+ 1 2)", 2);

    REQUIRE(t.parentOf(a) == 1);
    REQUIRE(t.parentOf(b) == 2);
}

TEST_CASE("Tree::envsOf")
{
    Tree t;
    SheetIndex sheet = 15; // dummy value

    SECTION("Single instance")
    {
        auto a = t.insertInstance("a", sheet, 0);

        auto envs = t.envsOf(sheet);
        REQUIRE(envs.size() == 1);

        auto f = envs.front();
        REQUIRE(f.size() == 2);
        REQUIRE(f.front() == 0);
        f.pop_front();
        REQUIRE(f.front() == a);
    }

    SECTION("Nested instances")
    {
        // Here, we create a 2-layer tree, with 2 then 3 branches
        SheetIndex parent = 57; // another dummy value
        auto i1 = t.insertInstance("a", sheet, parent);
        auto i2 = t.insertInstance("b", sheet, parent);
        auto i3 = t.insertInstance("c", sheet, parent);
        REQUIRE(i1 != i2);

        auto a = t.insertInstance("a", parent, 0);
        auto b = t.insertInstance("b", parent, 0);

        {
            auto envs = t.envsOf(sheet);
            REQUIRE(envs.size() == 6);
            REQUIRE(envs.front().size() == 3);
        }
        {
            auto envs = t.envsOf(parent);
            REQUIRE(envs.size() == 2);
            REQUIRE(envs.front().size() == 2);
        }
    }
}
