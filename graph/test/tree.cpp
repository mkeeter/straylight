#include "catch/catch.hpp"

#include "graph/tree.hpp"
#include "graph/root.hpp"

using namespace Graph;

TEST_CASE("Tree::canInsert")
{
    Tree s;

    REQUIRE(s.canInsert(Tree::ROOT_SHEET, "a"));
    REQUIRE(s.canInsert(Tree::ROOT_SHEET, "b"));
    REQUIRE(!s.canInsert(Tree::ROOT_SHEET, ""));
}

TEST_CASE("Tree::insertCell")
{
    Tree s;
    auto a = s.insertCell(Tree::ROOT_SHEET, "a", "(+ 1 2)");

    REQUIRE(a.i == 2);
    REQUIRE(!s.canInsert(Tree::ROOT_SHEET, "a"));
}

TEST_CASE("Tree::insertInstance")
{
    Tree s;
    auto a = s.insertInstance(Tree::ROOT_SHEET, "a", 0);

    REQUIRE(a.i == 2);
    REQUIRE(!s.canInsert(Tree::ROOT_SHEET, "a"));
}

TEST_CASE("Tree::rename")
{
    Tree s;
    auto a = s.insertInstance(Tree::ROOT_SHEET, "a", 0);
    auto b = s.insertCell(Tree::ROOT_SHEET, "b", "(+ 1 2)");

    s.rename(a, "c");
    s.rename(b, "d");

    REQUIRE(!s.canInsert(Tree::ROOT_SHEET, "c"));
    REQUIRE(!s.canInsert(Tree::ROOT_SHEET, "d"));
}

TEST_CASE("Tree::at(ItemIndex)")
{
    Tree s;
    auto a = s.insertInstance(Tree::ROOT_SHEET, "a", 0);
    auto b = s.insertCell(Tree::ROOT_SHEET, "b", "(+ 1 2)");

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
    auto a = s.insertInstance(Tree::ROOT_SHEET, "a", 0);
    auto b = s.insertCell(Tree::ROOT_SHEET, "b", "(+ 1 2)");

    s.at(Tree::ROOT_SHEET, "a");
    s.at(Tree::ROOT_SHEET, "b");
    REQUIRE(true); // Didn't crash!

    bool threw = false;
    try
    {
        auto c = s.at(Tree::ROOT_SHEET, "c");
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
    auto a = s.insertInstance(Tree::ROOT_SHEET, "a", 0);
    auto b = s.insertCell(Tree::ROOT_SHEET, "b", "(+ 1 2)");

    REQUIRE(s.hasItem(Tree::ROOT_SHEET, "a"));
    REQUIRE(s.hasItem(Tree::ROOT_SHEET, "b"));
    REQUIRE(!s.hasItem(Tree::ROOT_SHEET, "c"));
}

TEST_CASE("Tree::nameOf")
{
    Tree s;
    auto a = s.insertInstance(Tree::ROOT_SHEET, "a", 0);
    auto b = s.insertCell(Tree::ROOT_SHEET, "b", "(+ 1 2)");

    REQUIRE(s.nameOf(a) == "a");
    REQUIRE(s.nameOf(b) == "b");
}

TEST_CASE("Tree::iterItems")
{
    Tree t;

    SECTION("Top level")
    {
        auto a = t.insertInstance(Tree::ROOT_SHEET, "a", 0);
        auto b = t.insertCell(Tree::ROOT_SHEET, "b", "(+ 1 2)");
        auto iter = t.iterItems(Tree::ROOT_SHEET);
        REQUIRE(iter.size() == 2);
        REQUIRE(iter.front() == a);
        iter.pop_front();
        REQUIRE(iter.front() == b);

        auto i1 = t.iterItems(1);
        REQUIRE(i1.size() == 0);
    }

    SECTION("Nested")
    {
        // 3 and 4 are dummy sheets here
        auto a = t.insertInstance(3, "a", 0);
        auto b = t.insertCell(4, "b", "(+ 1 2)");

        auto i0 = t.iterItems(Tree::ROOT_SHEET);
        REQUIRE(i0.size() == 0);

        auto i1 = t.iterItems(3);
        REQUIRE(i1.size() == 1);
        REQUIRE(i1.front() == a);

        auto i2 = t.iterItems(4);
        REQUIRE(i2.size() == 1);
        REQUIRE(i2.front() == b);
    }
}

TEST_CASE("Tree::parentOf")
{
    Tree t;

    auto a = t.insertInstance(2, "a", 0);
    auto b = t.insertCell(3, "b", "(+ 1 2)");

    REQUIRE(t.parentOf(a) == 2);
    REQUIRE(t.parentOf(b) == 3);
}

TEST_CASE("Tree::envsOf")
{
    Tree t;
    SheetIndex sheet = 15; // dummy value

    SECTION("Single instance")
    {
        auto a = t.insertInstance(Tree::ROOT_SHEET, "a", sheet);

        auto envs = t.envsOf(sheet);
        REQUIRE(envs.size() == 1);

        auto f = envs.front();
        REQUIRE(f.size() == 2);
        REQUIRE(f.front() == Tree::ROOT_INSTANCE);
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

        auto a = t.insertInstance(Tree::ROOT_SHEET, "a", parent);
        auto b = t.insertInstance(Tree::ROOT_SHEET, "b", parent);

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
        auto a = t.insertInstance(Tree::ROOT_SHEET, "a", sheet);

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

        auto a = t.insertInstance(Tree::ROOT_SHEET, "a", parent);
        auto b = t.insertInstance(Tree::ROOT_SHEET, "b", parent);

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
    auto c = t.insertCell(Tree::ROOT_SHEET, "c", "12");
    REQUIRE(t.iterItems(Tree::ROOT_SHEET).size() == 1);
    t.erase(c);
    REQUIRE(t.iterItems(Tree::ROOT_SHEET).size() == 0);
}

TEST_CASE("Tree::nextName")
{
    Tree t;
    REQUIRE(t.nextName(SheetIndex(Tree::ROOT_SHEET), "c") == "c0");
    auto c = t.insertCell(Tree::ROOT_SHEET, "c0", "12");
    REQUIRE(t.nextName(SheetIndex(Tree::ROOT_SHEET), "c") == "c1");
    REQUIRE(t.nextName(SheetIndex(Tree::ROOT_SHEET), "prefix") == "prefix0");
}

TEST_CASE("Tree::canInsertInstance")
{
    Tree t;

    // Dummy sheet indices
    SheetIndex root(Tree::ROOT_SHEET);
    SheetIndex a(2);
    SheetIndex b(3);

    SECTION("Self")
    {
        auto ia = t.insertInstance(root, "ia", a);
        REQUIRE(!t.canInsertInstance(a, a));
    }

    SECTION("Nested")
    {
        auto ia = t.insertInstance(root, "ia", a);
        auto ib = t.insertInstance(a, "ib", b);
        REQUIRE(!t.canInsertInstance(b, a));
    }

    SECTION("Multiple insertions")
    {
        auto ia = t.insertInstance(root, "ia", a);
        REQUIRE(t.canInsertInstance(root, a));
    }
}

TEST_CASE("Tree::toString")
{
    Root r;

    SECTION("Empty file")
    {
        auto before = r.getTree().toString();
        CAPTURE(before);
        REQUIRE(r.loadString(before) == "");
        REQUIRE(r.getTree().toString() == before);
    }

    SECTION("Single cell")
    {
        r.insertCell(Tree::ROOT_SHEET, "x", "(+ 1 2)");
        auto before = r.getTree().toString();
        CAPTURE(before);
        REQUIRE(r.loadString(before) == "");
        REQUIRE(r.getTree().toString() == before);
    }

    SECTION("Clearing existing values")
    {
        r.insertCell(Tree::ROOT_SHEET, "x", "(+ 1 2)");
        auto before = r.getTree().toString();
        r.insertCell(Tree::ROOT_SHEET, "y", "15");

        CAPTURE(before);
        REQUIRE(r.loadString(before) == "");
        REQUIRE(r.getTree().toString() == before);
    }

    SECTION("Sheet")
    {
        auto s = r.insertSheet(Tree::ROOT_SHEET, "Sheet");
        auto a = r.insertCell(s, "a", "15");

        auto before = r.getTree().toString();
        CAPTURE(before);
        REQUIRE(r.loadString(before) == "");
        REQUIRE(r.getTree().toString() == before);
        REQUIRE(!r.canInsertSheet(Tree::ROOT_SHEET, "Sheet"));
    }

    SECTION("Preserving order")
    {
        // TODO once we can re-order tree
        auto a = r.insertCell(Tree::ROOT_SHEET, "a", "15");
        auto b = r.insertCell(Tree::ROOT_SHEET, "b", "15");

        REQUIRE(r.getTree().iterItems(Tree::ROOT_SHEET).front() == a);

        auto before = r.getTree().toString();
        CAPTURE(before);
        REQUIRE(r.loadString(before) == "");
        REQUIRE(r.getTree().toString() == before);

        REQUIRE(r.getTree().iterItems(Tree::ROOT_SHEET).front() == a);
    }
}

TEST_CASE("Tree::cellsOf")
{
    SECTION("Nested instances")
    {
        Root r;

        auto sum = r.insertSheet(Tree::ROOT_SHEET, "Sum");
        auto z = r.insertSheet(Tree::ROOT_SHEET, "Zero");
        auto is = r.insertInstance(Tree::ROOT_SHEET, "s", sum);
        auto iz = r.insertInstance(sum, "z-instance", z);
        auto c = r.insertCell(z, "a", "15");

        {   // Check with respect to root
            auto cs = r.getTree().cellsOf(Tree::ROOT_SHEET);
            REQUIRE(cs.size() == 1);

            auto cell = cs.front();
            REQUIRE(cell.first.size() == 2);
            REQUIRE(cell.second == c.i);
        }

        {   // Check with respect to inner sheet
            auto cs = r.getTree().cellsOf(sum);
            REQUIRE(cs.size() == 1);

            auto cell = cs.front();
            REQUIRE(cell.first.size() == 1);
            REQUIRE(cell.second == c.i);
        }
    }
}
