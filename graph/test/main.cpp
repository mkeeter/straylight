#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "root.hpp"
#include "cell.hpp"

TEST_CASE("Simple evaluation")
{
    Graph::Root root;
    auto c = root.insertCell(root.sheet.get(), "x", "(+ 1 2)");
    REQUIRE(c->values.size() == 1);
    REQUIRE(c->values.count({root.instance.get()}) == 1);
    REQUIRE(c->strs[{root.instance.get()}] == "3");
}

TEST_CASE("Evaluation with lookups")
{
    Graph::Root root;
    auto x = root.insertCell(root.sheet.get(), "x", "1");
    auto y = root.insertCell(root.sheet.get(), "y", "(+ 1 (x))");
    REQUIRE(y->strs[{root.instance.get()}] == "2");
}

TEST_CASE("Re-evaluation on parent change")
{
    Graph::Root root;
    auto x = root.insertCell(root.sheet.get(), "x", "1");
    auto y = root.insertCell(root.sheet.get(), "y", "(+ 1 (x))");
    root.editCell(x, "2");
    REQUIRE(y->strs[{root.instance.get()}] == "3");
}

TEST_CASE("Multiple evaluation paths")
{
    Graph::Root root;
    auto x = root.insertCell(root.sheet.get(), "x", "1");
    auto a = root.insertCell(root.sheet.get(), "a", "(+ 2 (x))");
    auto y = root.insertCell(root.sheet.get(), "y", "(+ (a) (x))");
    root.editCell(x, "3");
    REQUIRE(y->strs[{root.instance.get()}] == "8");
}

TEST_CASE("Re-evaluation on cell insertion")
{
    Graph::Root root;
    auto y = root.insertCell(root.sheet.get(), "y", "(+ 1 (x))");
    auto x = root.insertCell(root.sheet.get(), "x", "1");
    REQUIRE(y->strs[{root.instance.get()}] == "2");
}

TEST_CASE("Root::canInsert")
{
    Graph::Root root;
    SECTION("Cell name collision")
    {
        auto x = root.insertCell(root.sheet.get(), "x", "1");
        REQUIRE(!root.canInsert(root.sheet.get(), "x"));
        REQUIRE(root.canInsert(root.sheet.get(), "y"));
    }

    SECTION("Instance name collision")
    {
        auto s = root.createSheet(root.sheet.get(), "b");
        root.insertInstance(root.sheet.get(), "x", s);
        REQUIRE(!root.canInsert(root.sheet.get(), "x"));
        REQUIRE(root.canInsert(root.sheet.get(), "y"));
    }
}

TEST_CASE("Auto-recursive lookup")
{
    Graph::Root root;
    auto x = root.insertCell(root.sheet.get(), "x", "(x)");
    REQUIRE(!root.isValid(x));
}

TEST_CASE("Root::canCreateSheet")
{
    Graph::Root root;
    REQUIRE(root.canCreateSheet(root.sheet.get(), "b"));
    auto s = root.createSheet(root.sheet.get(), "b");
    REQUIRE(!root.canCreateSheet(root.sheet.get(), "b"));
    REQUIRE(root.canCreateSheet(root.sheet.get(), "a"));
}

TEST_CASE("Root::canInsertInstance")
{
    Graph::Root root;
    auto s = root.createSheet(root.sheet.get(), "b");
    REQUIRE(root.canInsertInstance(root.sheet.get(), "b", s));
    REQUIRE(!root.canInsertInstance(root.sheet.get(), "b", root.sheet.get()));
}

TEST_CASE("Root::rename")
{
    Graph::Root root;
    auto x = root.insertCell(root.sheet.get(), "x", "(+ 1 2)");
    auto z = root.insertCell(root.sheet.get(), "z", "(+ (y) 2)");
    root.rename(root.sheet.get(), "x", "y");
    REQUIRE(z->strs[{root.instance.get()}] == "5");
}

int main(int argc, char** argv)
{
    return Catch::Session().run(argc, argv);
}
