#include "catch/catch.hpp"

#include "graph/root.hpp"
#include "graph/cell.hpp"

TEST_CASE("Simple evaluation")
{
    Graph::Root root;
    auto c = root.insertCell(root.sheet.get(), "x", "(+ 1 2)");
    REQUIRE(c->values.size() == 1);
    REQUIRE(c->values.count({root.instance.get()}) == 1);
    REQUIRE(c->values[{root.instance.get()}].str == "3");
}

TEST_CASE("Evaluation with lookups")
{
    Graph::Root root;
    auto x = root.insertCell(root.sheet.get(), "x", "1");
    auto y = root.insertCell(root.sheet.get(), "y", "(+ 1 (x))");
    REQUIRE(y->values[{root.instance.get()}].str == "2");
}

TEST_CASE("Re-evaluation on parent change")
{
    Graph::Root root;
    auto x = root.insertCell(root.sheet.get(), "x", "1");
    auto y = root.insertCell(root.sheet.get(), "y", "(+ 1 (x))");
    root.editCell(x, "2");
    REQUIRE(y->values[{root.instance.get()}].str == "3");
}

TEST_CASE("Parent becoming invalid")
{
    Graph::Root root;
    auto x = root.insertCell(root.sheet.get(), "x", "(+ 1 2)");
    auto y = root.insertCell(root.sheet.get(), "y", "(+ 1 (x))");
    root.editCell(x, "(+ 1 2");
    REQUIRE(!y->values[{root.instance.get()}].valid);
}

TEST_CASE("Multiple evaluation paths")
{
    Graph::Root root;
    auto x = root.insertCell(root.sheet.get(), "x", "1");
    auto a = root.insertCell(root.sheet.get(), "a", "(+ 2 (x))");
    auto y = root.insertCell(root.sheet.get(), "y", "(+ (a) (x))");
    root.editCell(x, "3");
    REQUIRE(y->values[{root.instance.get()}].str == "8");
}

TEST_CASE("Re-evaluation on cell insertion")
{
    Graph::Root root;
    auto y = root.insertCell(root.sheet.get(), "y", "(+ 1 (x))");
    auto x = root.insertCell(root.sheet.get(), "x", "1");
    REQUIRE(y->values[{root.instance.get()}].str == "2");
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

    SECTION("Valid and invalid names")
    {
        REQUIRE(root.canInsert(root.sheet.get(), "x"));
        REQUIRE(root.canInsert(root.sheet.get(), "name-with-dashes"));
        REQUIRE(root.canInsert(root.sheet.get(), "a123"));
        REQUIRE(root.canInsert(root.sheet.get(), "123a"));
        REQUIRE(!root.canInsert(root.sheet.get(), "123"));
        REQUIRE(!root.canInsert(root.sheet.get(), "name with spaces"));
        REQUIRE(!root.canInsert(root.sheet.get(), "trailing-space "));
        REQUIRE(!root.canInsert(root.sheet.get(), "trailing-spaces   "));
        REQUIRE(!root.canInsert(root.sheet.get(), "    preceeding-spaces"));
        REQUIRE(!root.canInsert(root.sheet.get(), "(name-with-parens)"));
        REQUIRE(!root.canInsert(root.sheet.get(), ""));
    }
}

TEST_CASE("Cell.values.valid")
{
    Graph::Root root;
    SECTION("Auto-recursive lookup")
    {
        auto x = root.insertCell(root.sheet.get(), "x", "(x)");
        REQUIRE(!x->values[{root.instance.get()}].valid);
    }
}

TEST_CASE("Root::canCreateSheet")
{
    Graph::Root root;
    REQUIRE(root.canCreateSheet(root.sheet.get(), "b"));
    auto s = root.createSheet(root.sheet.get(), "b");
    REQUIRE(!root.canCreateSheet(root.sheet.get(), "b"));
    REQUIRE(!root.canCreateSheet(root.sheet.get(), ""));
    REQUIRE(root.canCreateSheet(root.sheet.get(), "a"));
}

TEST_CASE("Root::canInsertInstance")
{
    Graph::Root root;

    SECTION("Recursive")
    {
        auto a = root.createSheet(root.sheet.get(), "a");
        auto ia = root.insertInstance(root.sheet.get(), "ia", a);
        REQUIRE(!root.canInsertInstance(ia->sheet, a));
    }

    SECTION("More recursive")
    {
        auto a = root.createSheet(root.sheet.get(), "a");
        auto ia = root.insertInstance(root.sheet.get(), "ia", a);
        auto b = root.createSheet(ia->sheet, "b");
        auto ib = root.insertInstance(ia->sheet, "ib", b);
        REQUIRE(!root.canInsertInstance(ib->sheet, b));
        REQUIRE(!root.canInsertInstance(ib->sheet, a));
    }
}

TEST_CASE("Root::rename")
{
    Graph::Root root;
    SECTION("Renaming")
    {
        auto x = root.insertCell(root.sheet.get(), "x", "(+ 1 2)");
        root.rename(root.sheet.get(), "x", "y");
        REQUIRE(root.sheet->cells.left.count("y") == 1);
        REQUIRE(root.sheet->cells.left.at("y") == x);
    }

    SECTION("Change detection")
    {
        auto x = root.insertCell(root.sheet.get(), "x", "(+ 1 2)");

        auto a = root.insertCell(root.sheet.get(), "a", "(+ 1 (x))");
        REQUIRE(a->values[{root.instance.get()}].str == "4");
        REQUIRE(a->values[{root.instance.get()}].valid);

        auto b = root.insertCell(root.sheet.get(), "b", "(+ (y) 2)");
        REQUIRE(!b->values[{root.instance.get()}].valid);

        root.rename(root.sheet.get(), "x", "y");
        REQUIRE(!a->values[{root.instance.get()}].valid);
        REQUIRE(b->values[{root.instance.get()}].str == "5");
        REQUIRE(b->values[{root.instance.get()}].valid);
    }

    SECTION("Renaming instances")
    {
        auto a = root.createSheet(root.sheet.get(), "a");
        auto ia = root.insertInstance(root.sheet.get(), "ia", a);
        root.rename(root.sheet.get(), "ia", "ja");
        REQUIRE(root.sheet->instances.left.count("ja") == 1);
        REQUIRE(root.sheet->instances.left.at("ja") == ia);
    }
}

TEST_CASE("Root::eraseCell")
{
    Graph::Root root;

    SECTION("Basic erasing")
    {
        auto x = root.insertCell(root.sheet.get(), "x", "(+ 1 2)");
        REQUIRE(root.sheet->cells.right.count(x));
        root.eraseCell(x);
        REQUIRE(!root.sheet->cells.right.count(x));
    }

    SECTION("Change detection")
    {
        auto x = root.insertCell(root.sheet.get(), "x", "(+ 1 2)");
        auto a = root.insertCell(root.sheet.get(), "a", "(+ 1 (x))");

        root.eraseCell(x);
        REQUIRE(!a->values[{root.instance.get()}].valid);
    }

    SECTION("Dependency cleanup")
    {
        auto x = root.insertCell(root.sheet.get(), "x", "(+ 1 2)");
        auto a = root.insertCell(root.sheet.get(), "a", "(+ 1 (x))");

        root.eraseCell(a);
        root.editCell(x, "15");
        REQUIRE(x->values[{root.instance.get()}].str == "15");
    }
}

TEST_CASE("Root::createSheet")
{
    Graph::Root root;
    auto a = root.createSheet(root.sheet.get(), "a");
    auto b = root.createSheet(root.sheet.get(), "b");

    REQUIRE(a != nullptr);
    REQUIRE(a->parent == root.sheet.get());
    REQUIRE(b != nullptr);
    REQUIRE(b->parent == root.sheet.get());
}

TEST_CASE("Root::insertInstance")
{
    Graph::Root root;

    SECTION("Flat")
    {
        auto a = root.createSheet(root.sheet.get(), "a");
        auto b = root.createSheet(root.sheet.get(), "b");

        auto ia = root.insertInstance(root.sheet.get(), "ia", a);
        auto ib = root.insertInstance(root.sheet.get(), "ib", b);

        REQUIRE(ia->sheet == a);
        REQUIRE(ib->sheet == b);
        REQUIRE(ia->sheet->parent == root.sheet.get());
        REQUIRE(ib->sheet->parent == root.sheet.get());
        REQUIRE(ia->parent == root.sheet.get());
        REQUIRE(ib->parent == root.sheet.get());
    }

    SECTION("Nested")
    {
        auto a = root.createSheet(root.sheet.get(), "a");
        auto ia = root.insertInstance(root.sheet.get(), "ia", a);
        auto ab = root.createSheet(ia->sheet, "b");
        auto iab = root.insertInstance(ia->sheet, "ib", ab);

        auto c = root.createSheet(root.sheet.get(), "c");
        auto iac = root.insertInstance(ia->sheet, "iac", c);
        auto iabc = root.insertInstance(iab->sheet, "iabc", c);

        REQUIRE(a->parent == root.sheet.get());
        REQUIRE(ia->parent == root.sheet.get());
        REQUIRE(ab->parent == a);
        REQUIRE(iab->parent == a);
        REQUIRE(c->parent == root.sheet.get());
        REQUIRE(iac->parent == ia->sheet);
        REQUIRE(iabc->parent == iab->sheet);
    }

    SECTION("Change detection")
    {
        auto x = root.insertCell(root.sheet.get(), "x", "(ia 'x)");
        REQUIRE(x->values[{root.instance.get()}].str == "ia: unbound variable");

        auto a = root.createSheet(root.sheet.get(), "a");
        auto ia = root.insertInstance(root.sheet.get(), "ia", a);

        REQUIRE(x->values[{root.instance.get()}].str == "x: missing instance lookup in ia");
    }
}

TEST_CASE("Root: inputs")
{
    Graph::Root root;

    auto a = root.createSheet(root.sheet.get(), "a");
    auto x = root.insertCell(a, "x", "(input 0)");

    auto ia = root.insertInstance(root.sheet.get(), "ia", a);
    Graph::Env env = {root.instance.get(), ia};

    SECTION("Evaluation")
    {
        REQUIRE(x->values.size() == 1);
        REQUIRE(x->values.count(env) == 1);
        REQUIRE(x->values[env].str == "0");

        REQUIRE(ia->inputs.count(x));
        REQUIRE(ia->inputs.at(x) == "0");
    }

    SECTION("References")
    {
        auto y = root.insertCell(a, "y", "(+ 1 (x))");
        REQUIRE(y->values.size() == 1);
        REQUIRE(y->values.count(env) == 1);
        REQUIRE(y->values[env].str == "1");
    }
}

TEST_CASE("Root: outputs")
{
    Graph::Root root;

    auto a = root.createSheet(root.sheet.get(), "a");
    auto x = root.insertCell(a, "x", "(output 0)");

    auto ia = root.insertInstance(root.sheet.get(), "ia", a);

    SECTION("Evaluation")
    {
        Graph::Env env = {root.instance.get(), ia};

        REQUIRE(x->values.size() == 1);
        REQUIRE(x->values.count(env) == 1);
        REQUIRE(x->values[env].str == "0");
    }

    SECTION("Valid reference")
    {
        auto y = root.insertCell(root.sheet.get(), "y", "(+ 1 (ia 'x))");

        Graph::Env env = {root.instance.get()};
        REQUIRE(y->values.size() == 1);
        REQUIRE(y->values.count(env) == 1);
        REQUIRE(y->values[env].str == "1");
    }

    SECTION("Invalid reference")
    {
        auto y = root.insertCell(root.sheet.get(), "y", "(+ 1 (ia 'z))");

        Graph::Env env = {root.instance.get()};
        REQUIRE(y->values.size() == 1);
        REQUIRE(y->values.count(env) == 1);
        REQUIRE(y->values[env].str == "z: missing instance lookup in ia");
    }

    SECTION("Change detection")
    {
        root.editCell(x, "12");
        auto y = root.insertCell(root.sheet.get(), "y", "(+ 1 (ia 'x))");

        Graph::Env env = {root.instance.get()};
        CAPTURE(y->values[env].str);
        REQUIRE(y->values[env].valid == false);

        root.editCell(x, "(output 15)");
        CAPTURE(y->values[env].str);
        REQUIRE(y->values[env].valid == true);
        REQUIRE(y->values[env].str == "16");
    }
}

TEST_CASE("Root::editInput")
{
    Graph::Root root;

    auto a = root.createSheet(root.sheet.get(), "a");
    auto x = root.insertCell(a, "x", "(input 0)");

    auto ia = root.insertInstance(root.sheet.get(), "ia", a);

    root.editInput(ia, x, "12");

    Graph::Env env = {root.instance.get(), ia};
    REQUIRE(x->values[env].str == "12");
}
