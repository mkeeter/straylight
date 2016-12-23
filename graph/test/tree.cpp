#include "catch/catch.hpp"

#include "graph/tree.hpp"

TEST_CASE("Tree::canInsert")
{
    Tree s;

    REQUIRE(s.canInsert(0, "a"));
    REQUIRE(s.canInsert(0, "b"));
    REQUIRE(!s.canInsert(0, ""));
}

TEST_CASE("Tree::insertCell")
{
    Tree s;
    auto a = s.insertCell(0, "a", "(+ 1 2)");

    REQUIRE(a.i == 1);
    REQUIRE(!s.canInsert(0, "a"));
}

TEST_CASE("Tree::insertInstance")
{
    Tree s;
    auto a = s.insertInstance(0, "a", 0);

    REQUIRE(a.i == 1);
    REQUIRE(!s.canInsert(0, "a"));
}

TEST_CASE("Tree::rename")
{
    Tree s;
    auto a = s.insertInstance(0, "a", 0);
    auto b = s.insertCell(0, "b", "(+ 1 2)");

    s.rename(a, "c");
    s.rename(b, "d");

    REQUIRE(!s.canInsert(0, "c"));
    REQUIRE(!s.canInsert(0, "d"));
}

TEST_CASE("Tree::at(ItemIndex)")
{
    Tree s;
    auto a = s.insertInstance(0, "a", 0);
    auto b = s.insertCell(0, "b", "(+ 1 2)");

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
    auto a = s.insertInstance(0, "a", 0);
    auto b = s.insertCell(0, "b", "(+ 1 2)");

    s.at(0, "a");
    s.at(0, "b");
    REQUIRE(true); // Didn't crash!

    bool threw = false;
    try
    {
        auto c = s.at(0, "c");
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
    auto a = s.insertInstance(0, "a", 0);
    auto b = s.insertCell(0, "b", "(+ 1 2)");

    REQUIRE(s.hasItem(0, "a"));
    REQUIRE(s.hasItem(0, "b"));
    REQUIRE(!s.hasItem(0, "c"));
}

TEST_CASE("Tree::nameOf")
{
    Tree s;
    auto a = s.insertInstance(0, "a", 0);
    auto b = s.insertCell(0, "b", "(+ 1 2)");

    REQUIRE(s.nameOf(a) == "a");
    REQUIRE(s.nameOf(b) == "b");
}

TEST_CASE("Tree::iterItems")
{
    Tree t;

    SECTION("Top level")
    {
        auto a = t.insertInstance(0, "a", 0);
        auto b = t.insertCell(0, "b", "(+ 1 2)");
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
        auto a = t.insertInstance(1, "a", 0);
        auto b = t.insertCell(2, "b", "(+ 1 2)");

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

    auto a = t.insertInstance(1, "a", 0);
    auto b = t.insertCell(2, "b", "(+ 1 2)");

    REQUIRE(t.parentOf(a) == 1);
    REQUIRE(t.parentOf(b) == 2);
}

TEST_CASE("Tree::envsOf")
{
    Tree t;
    SheetIndex sheet = 15; // dummy value

    SECTION("Single instance")
    {
        auto a = t.insertInstance(0, "a", sheet);

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
        auto i1 = t.insertInstance(parent, "a", sheet);
        auto i2 = t.insertInstance(parent, "b", sheet);
        auto i3 = t.insertInstance(parent, "c", sheet);
        REQUIRE(i1 != i2);

        auto a = t.insertInstance(0, "a", parent);
        auto b = t.insertInstance(0, "b", parent);

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

TEST_CASE("Tree::instancesOf")
{
    Tree t;
    SheetIndex sheet = 15; // dummy value

    SECTION("Single instance")
    {
        auto a = t.insertInstance(0, "a", sheet);

        auto ins = t.instancesOf(sheet);
        REQUIRE(ins.size() == 1);
        REQUIRE(ins.front() == a);
    }

    SECTION("Nested instances")
    {
        // Here, we create a 2-layer tree, with 2 then 3 branches
        SheetIndex parent = 57; // another dummy value
        auto i1 = t.insertInstance(parent, "a", sheet);
        auto i2 = t.insertInstance(parent, "b", sheet);
        auto i3 = t.insertInstance(parent, "c", sheet);
        REQUIRE(i1 != i2);

        auto a = t.insertInstance(0, "a", parent);
        auto b = t.insertInstance(0, "b", parent);

        {
            auto ins = t.instancesOf(sheet);
            REQUIRE(ins.size() == 3);
        }
        {
            auto ins = t.instancesOf(parent);
            REQUIRE(ins.size() == 2);
        }
    }
}

TEST_CASE("Tree::erase")
{
    Tree t;
    auto c = t.insertCell(0, "c", "12");
    REQUIRE(t.iterItems(0).size() == 1);
    t.erase(c);
    REQUIRE(t.iterItems(0).size() == 0);
}
