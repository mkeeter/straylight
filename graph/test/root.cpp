#include "catch/catch.hpp"

#include "graph/root.hpp"

using namespace Graph;

TEST_CASE("Root::toCellKey")
{
    Root r;
    auto a = r.insertCell(0, "a", "(+ 1 2)");
    auto ck = r.toCellKey({{0}, "a"});
    REQUIRE(ck.first.size() == 1);
    REQUIRE(ck.first.front() == 0);
    REQUIRE(ck.second == a);
}

TEST_CASE("Root::toNameKey")
{
    Root r;
    auto a = r.insertCell(0, "a", "(+ 1 2)");
    auto nk = r.toNameKey({{0}, a});
    REQUIRE(nk.first.size() == 1);
    REQUIRE(nk.first.front() == 0);
    REQUIRE(nk.second == "a");
}

TEST_CASE("Root::getValue")
{
    Root r;
    auto a = r.insertCell(0, "a", "(+ 1 2)");
    auto v = r.getValue({{0}, a});
    REQUIRE(v.value != nullptr);
    REQUIRE(v.valid == true);
    REQUIRE(v.str == "3");
}

TEST_CASE("Root::insertCell")
{
    Root r;

    SECTION("Basic")
    {
        auto a = r.insertCell(0, "a", "(+ 1 2)");
        auto v = r.getValue({{0}, a});
        REQUIRE(v.value != nullptr);
        REQUIRE(v.valid == true);
        REQUIRE(v.str == "3");
    }

    SECTION("With lookups (ordered)")
    {
        auto x = r.insertCell(0, "x", "1");
        auto y = r.insertCell(0, "y", "(+ 1 (x))");
        auto v = r.getValue({{0}, y});
        REQUIRE(v.value != nullptr);
        REQUIRE(v.valid == true);
        REQUIRE(v.str == "2");
    }

    SECTION("With lookups (unordered)")
    {
        auto y = r.insertCell(0, "y", "(+ 1 (x))");
        auto x = r.insertCell(0, "x", "1");
        auto v = r.getValue({{0}, y});
        REQUIRE(v.value != nullptr);
        REQUIRE(v.valid == true);
        REQUIRE(v.str == "2");
    }

    SECTION("Single-item circular lookup")
    {
        auto a = r.insertCell(0, "a", "(a)");
        auto v = r.getValue({{0}, a});
        REQUIRE(v.value != nullptr);
        REQUIRE(v.valid == false);
        REQUIRE(v.str == "Circular lookup");
    }
}

TEST_CASE("Root::renameItem")
{
    Root r;

    SECTION("Change tracking")
    {
        auto a = r.insertCell(0, "a", "12");
        auto b = r.insertCell(0, "b", "(/ (d) 2)");
        auto c = r.insertCell(0, "c", "(+ (a) 5)");

        r.renameItem(a, "d");

        auto vb = r.getValue({{0}, b});
        REQUIRE(vb.value != nullptr);
        REQUIRE(vb.valid == true);
        REQUIRE(vb.str == "6");

        auto vc = r.getValue({{0}, c});
        REQUIRE(vc.value != nullptr);
        REQUIRE(vc.valid == false);
    }

    SECTION("Renaming to self")
    {
        auto a = r.insertCell(0, "a", "12");
        r.renameItem(a, "a");
        REQUIRE(true /* didn't crash */);
    }
}

TEST_CASE("Root::eraseCell")
{
    Root r;

    SECTION("Basic erasing")
    {
        auto x = r.insertCell(0, "x", "(+ 1 2)");
        REQUIRE(!r.checkItemName(0, "x"));
        r.eraseCell(x);
        REQUIRE(r.checkItemName(0, "x"));
    }

    SECTION("Change detection")
    {
        auto x = r.insertCell(0, "x", "(+ 1 2)");
        auto a = r.insertCell(0, "a", "(+ 1 (x))");

        r.eraseCell(x);
        REQUIRE(!r.getValue({{0}, a}).valid);
    }

    SECTION("Dependency cleanup")
    {
        auto x = r.insertCell(0, "x", "(+ 1 2)");
        auto a = r.insertCell(0, "a", "(+ 1 (x))");

        r.eraseCell(a);
        r.setExpr(x, "15");
        REQUIRE(r.getValue({{0}, x}).str == "15");
    }
}

TEST_CASE("Root::setExpr")
{
    Root r;

    SECTION("Return value")
    {
        auto x = r.insertCell(0, "x", "1");
        REQUIRE(!r.setExpr(x, "1"));
        REQUIRE(r.setExpr(x, "2"));
    }

    SECTION("Re-evaluation on parent change")
    {
        auto x = r.insertCell(0, "x", "1");
        auto y = r.insertCell(0, "y", "(+ 1 (x))");

        r.setExpr(x, "10");
        auto v = r.getValue({{0}, y});
        REQUIRE(v.value != nullptr);
        REQUIRE(v.valid == true);
        REQUIRE(v.str == "11");
    }

    SECTION("Upstream becoming invalid")
    {
        auto x = r.insertCell(0, "x", "1");
        auto y = r.insertCell(0, "y", "(+ 1 (x))");

        r.setExpr(x, "this isn't a valid expression");
        auto v = r.getValue({{0}, y});
        REQUIRE(v.value != nullptr);
        REQUIRE(v.valid == false);
        REQUIRE(v.str == "Invalid lookup");
    }

    SECTION("Multiple evaluation paths")
    {
        auto x = r.insertCell(0, "x", "1");
        r.insertCell(0, "a", "(+ 2 (x))");
        auto y = r.insertCell(0, "y", "(+ (a) (x))");
        r.setExpr(x, "3");

        auto v = r.getValue({{0}, y});
        REQUIRE(v.value != nullptr);
        REQUIRE(v.valid == true);
        REQUIRE(v.str == "8");
    }

    SECTION("Cell becoming an output")
    {
        auto sum = r.insertSheet(0, "Sum");

        auto a = r.insertCell(sum, "a", "10");
        auto i = r.insertInstance(0, "i", sum);

        auto b = r.insertCell(0, "b", "(+ 1 (i 'a))");
        r.setExpr(a, "(output 5)");

        REQUIRE(r.getValue({{0}, b}).str == "6");
    }

    SECTION("Cell becoming an output (same value)")
    {
        auto sum = r.insertSheet(0, "Sum");

        auto a = r.insertCell(sum, "a", "10");
        auto i = r.insertInstance(0, "i", sum);

        auto b = r.insertCell(0, "b", "(+ 1 (i 'a))");
        r.setExpr(a, "(output 10)");

        REQUIRE(r.getValue({{0}, b}).str == "11");
    }
}

TEST_CASE("Root::canInsertSheet")
{
    Root r;
    REQUIRE(r.canInsertSheet(0, "Sum"));
    REQUIRE(!r.canInsertSheet(0, "sum"));
    REQUIRE(!r.canInsertSheet(0, ""));

    REQUIRE(r.canInsertSheet(0, "A"));
    REQUIRE(r.canInsertSheet(0, "B"));
    REQUIRE(r.canInsertSheet(0, "C"));

    REQUIRE(!r.canInsertSheet(0, ""));
    REQUIRE(!r.canInsertSheet(0, "a"));
    REQUIRE(!r.canInsertSheet(0, "b"));
    REQUIRE(!r.canInsertSheet(0, "c"));
    REQUIRE(!r.canInsertSheet(0, "12c"));
    REQUIRE(!r.canInsertSheet(0, "with a space"));

    r.insertSheet(0, "Sum");
    REQUIRE(!r.canInsertSheet(0, "Sum"));
}

TEST_CASE("Root::insertSheet")
{
    Root r;
    auto sum = r.insertSheet(0, "Sum");
    REQUIRE(sum.i == 1);

    auto mul = r.insertSheet(0, "Mul");
    REQUIRE(mul.i == 2);
}

TEST_CASE("Root::eraseSheet")
{
    Root r;
    auto sum = r.insertSheet(0, "Sum");

    auto i = r.insertInstance(0, "i", sum);
    REQUIRE(!r.checkItemName(0, "i"));

    r.eraseSheet(sum);
    REQUIRE(r.checkItemName(0, "i"));
}

TEST_CASE("Root::insertInstance")
{
    Root r;

    auto sum = r.insertSheet(0, "Sum");

    SECTION("Cells within sheet")
    {
        auto a = r.insertCell(sum, "a", "1");
        auto b = r.insertCell(sum, "b", "2");
        auto out = r.insertCell(sum, "out", "(+ (a) (b))");

        auto i = r.insertInstance(0, "instance", sum);
        REQUIRE(r.getValue({{0, i}, out}).str == "3");
    }

    SECTION("Multiple instances")
    {
        auto a = r.insertCell(sum, "a", "1");
        auto b = r.insertCell(sum, "b", "2");
        auto out = r.insertCell(sum, "out", "(+ (a) (b))");

        auto i = r.insertInstance(0, "instance", sum);
        auto j = r.insertInstance(0, "jnstance", sum);
        REQUIRE(r.getValue({{0, i}, out}).value !=
                r.getValue({{0, j}, out}).value);
    }

    SECTION("Sheet input default")
    {
        auto a = r.insertCell(sum, "a", "(input 10)");
        auto b = r.insertCell(sum, "b", "2");
        auto out = r.insertCell(sum, "out", "(+ (a) (b))");

        auto i = r.insertInstance(0, "instance", sum);
        REQUIRE(r.getValue({{0, i}, out}).str == "12");
    }

    SECTION("Input evaluation environment")
    {
        auto input = r.insertCell(sum, "in", "(input (+ 1 (a)))");
        auto i = r.insertInstance(0, "instance", sum);

        auto a = r.insertCell(0, "a", "12");
        REQUIRE(r.getValue({{0, i}, input}).str == "13");

        r.setExpr(a, "13");
        REQUIRE(r.getValue({{0, i}, input}).str == "14");
    }

    SECTION("Output values")
    {
        auto a = r.insertCell(sum, "a", "(output 10)");
        auto i = r.insertInstance(0, "i", sum);

        auto b = r.insertCell(0, "b", "(+ 1 (i 'a))");
        REQUIRE(r.getValue({{0}, b}).str == "11");
    }

    SECTION("Detecting a new instance + output")
    {
        auto b = r.insertCell(0, "b", "(+ 1 (i 'a))");

        auto a = r.insertCell(sum, "a", "(output 10)");
        auto i = r.insertInstance(0, "i", sum);

        REQUIRE(r.getValue({{0}, b}).str == "11");
    }
}

TEST_CASE("Root::setInput")
{
    Root r;

    auto sum = r.insertSheet(0, "Sum");

    auto a = r.insertCell(sum, "a", "(input 10)");
    auto b = r.insertCell(sum, "b", "2");
    auto out = r.insertCell(sum, "out", "(+ (a) (b))");

    auto i = r.insertInstance(0, "instance", sum);
    r.setInput(i, a, "12");

    REQUIRE(r.getValue({{0, i}, out}).str == "14");
}

TEST_CASE("Root::eraseInstance")
{
    Root r;

    auto sum = r.insertSheet(0, "Sum");

    SECTION("Re-evaluating outputs")
    {
        auto b = r.insertCell(0, "b", "(+ 1 (i 'a))");

        auto a = r.insertCell(sum, "a", "(output 10)");
        auto i = r.insertInstance(0, "i", sum);

        r.eraseInstance(i);
        CAPTURE(r.getValue({{0}, b}).str);
        REQUIRE(r.getValue({{0}, b}).valid == false);
    }

    SECTION("Cleaning of values")
    {
        auto a = r.insertCell(sum, "a", " 12");
        REQUIRE(r.getItem(a).cell()->values.size() == 0);

        // Insert an instance; one value should be created
        auto i = r.insertInstance(0, "instance", sum);
        REQUIRE(r.getItem(a).cell()->values.size() == 1);

        // After erasing the instance, that values should be cleaned up
        r.eraseInstance(i);
        REQUIRE(r.getItem(a).cell()->values.size() == 0);
    }

    SECTION("Cleaning of dependency list")
    {
        auto input = r.insertCell(sum, "in", "(input (+ 1 (a)))");
        auto i = r.insertInstance(0, "instance", sum);

        auto a = r.insertCell(0, "a", "12"); // used in input
        r.eraseInstance(i);

        r.setExpr(a, "13");
        /*  If this crashes, then the test fails!  */
    }
}

TEST_CASE("Root::clear")
{
    Root r;

    SECTION("Basic")
    {
        auto a = r.insertCell(0, "a", "(+ 1 2)");
        r.clear();
        REQUIRE(true /* didn't crash */);
    }

    SECTION("With inputs")
    {
        auto sum = r.insertSheet(0, "Sum");
        auto input = r.insertCell(sum, "in", "(input (+ 1 (a)))");
        auto i = r.insertInstance(0, "instance", sum);

        auto a = r.insertCell(0, "a", "12"); // used in input
        r.clear();
        REQUIRE(true /* didn't crash */);
    }
}

TEST_CASE("Root::fromString")
{
    Root r;

    // For now, just check that a few invalid strings return errors
    REQUIRE(r.fromString("HI THERE") != "");
    REQUIRE(r.fromString("{}") != "");
    REQUIRE(r.fromString("{") != "");
}

TEST_CASE("Root::toString")
{
    Root r;

    SECTION("Empty file")
    {
        auto before = r.toString();
        CAPTURE(before);
        REQUIRE(r.fromString(before) == "");
        REQUIRE(r.toString() == before);
    }

    SECTION("Single cell")
    {
        r.insertCell(0, "x", "(+ 1 2)");
        auto before = r.toString();
        CAPTURE(before);
        REQUIRE(r.fromString(before) == "");
        REQUIRE(r.toString() == before);
    }

    SECTION("Clearing existing values")
    {
        r.insertCell(0, "x", "(+ 1 2)");
        auto before = r.toString();
        r.insertCell(0, "y", "15");

        CAPTURE(before);
        REQUIRE(r.fromString(before) == "");
        REQUIRE(r.toString() == before);
    }

    SECTION("Sheet")
    {
        auto s = r.insertSheet(0, "Sheet");
        auto a = r.insertCell(s, "a", "15");

        auto before = r.toString();
        CAPTURE(before);
        REQUIRE(r.fromString(before) == "");
        REQUIRE(r.toString() == before);
    }
}
