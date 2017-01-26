#include "catch/catch.hpp"

#include "graph/root.hpp"

using namespace Graph;

TEST_CASE("Root::toCellKey")
{
    Root r;
    auto a = r.insertCell(Tree::ROOT_SHEET, "a", "(+ 1 2)");
    auto ck = r.toCellKey({{Tree::ROOT_INSTANCE}, "a"});
    REQUIRE(ck.first.size() == 1);
    REQUIRE(ck.first.front() == Tree::ROOT_INSTANCE);
    REQUIRE(ck.second == a);
}

TEST_CASE("Root::toNameKey")
{
    Root r;
    auto a = r.insertCell(Tree::ROOT_SHEET, "a", "(+ 1 2)");
    auto nk = r.toNameKey({{Tree::ROOT_INSTANCE}, a});
    REQUIRE(nk.first.size() == 1);
    REQUIRE(nk.first.front() == Tree::ROOT_INSTANCE);
    REQUIRE(nk.second == "a");
}

TEST_CASE("Root::getValue")
{
    Root r;
    auto a = r.insertCell(Tree::ROOT_SHEET, "a", "(+ 1 2)");
    auto v = r.getValue({{Tree::ROOT_INSTANCE}, a});
    REQUIRE(v.value != nullptr);
    REQUIRE(v.valid == true);
    REQUIRE(v.str == "3");
}

TEST_CASE("Root::insertCell")
{
    Root r;

    SECTION("Basic")
    {
        auto a = r.insertCell(Tree::ROOT_SHEET, "a", "(+ 1 2)");
        auto v = r.getValue({{Tree::ROOT_INSTANCE}, a});
        REQUIRE(v.value != nullptr);
        REQUIRE(v.valid == true);
        REQUIRE(v.str == "3");
    }

    SECTION("With lookups (ordered)")
    {
        auto x = r.insertCell(Tree::ROOT_SHEET, "x", "1");
        auto y = r.insertCell(Tree::ROOT_SHEET, "y", "(+ 1 (x))");
        auto v = r.getValue({{Tree::ROOT_INSTANCE}, y});
        REQUIRE(v.value != nullptr);
        REQUIRE(v.valid == true);
        REQUIRE(v.str == "2");
    }

    SECTION("With lookups (unordered)")
    {
        auto y = r.insertCell(Tree::ROOT_SHEET, "y", "(+ 1 (x))");
        auto x = r.insertCell(Tree::ROOT_SHEET, "x", "1");
        auto v = r.getValue({{Tree::ROOT_INSTANCE}, y});
        REQUIRE(v.value != nullptr);
        REQUIRE(v.valid == true);
        REQUIRE(v.str == "2");
    }

    SECTION("Single-item circular lookup")
    {
        auto a = r.insertCell(Tree::ROOT_SHEET, "a", "(a)");
        auto v = r.getValue({{Tree::ROOT_INSTANCE}, a});
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
        auto a = r.insertCell(Tree::ROOT_SHEET, "a", "12");
        auto b = r.insertCell(Tree::ROOT_SHEET, "b", "(/ (d) 2)");
        auto c = r.insertCell(Tree::ROOT_SHEET, "c", "(+ (a) 5)");

        r.renameItem(a, "d");

        auto vb = r.getValue({{Tree::ROOT_INSTANCE}, b});
        REQUIRE(vb.value != nullptr);
        REQUIRE(vb.valid == true);
        REQUIRE(vb.str == "6");

        auto vc = r.getValue({{Tree::ROOT_INSTANCE}, c});
        REQUIRE(vc.value != nullptr);
        REQUIRE(vc.valid == false);
    }

    SECTION("Renaming to self")
    {
        auto a = r.insertCell(Tree::ROOT_SHEET, "a", "12");
        r.renameItem(a, "a");
        REQUIRE(true /* didn't crash */);
    }
}

TEST_CASE("Root::eraseCell")
{
    Root r;

    SECTION("Basic erasing")
    {
        auto x = r.insertCell(Tree::ROOT_SHEET, "x", "(+ 1 2)");
        REQUIRE(!r.checkItemName(Tree::ROOT_SHEET, "x"));
        r.eraseCell(x);
        REQUIRE(r.checkItemName(Tree::ROOT_SHEET, "x"));
    }

    SECTION("Change detection")
    {
        auto x = r.insertCell(Tree::ROOT_SHEET, "x", "(+ 1 2)");
        auto a = r.insertCell(Tree::ROOT_SHEET, "a", "(+ 1 (x))");

        r.eraseCell(x);
        REQUIRE(!r.getValue({{Tree::ROOT_INSTANCE}, a}).valid);
    }

    SECTION("Dependency cleanup")
    {
        auto x = r.insertCell(Tree::ROOT_SHEET, "x", "(+ 1 2)");
        auto a = r.insertCell(Tree::ROOT_SHEET, "a", "(+ 1 (x))");

        r.eraseCell(a);
        r.setExpr(x, "15");
        REQUIRE(r.getValue({{Tree::ROOT_INSTANCE}, x}).str == "15");
    }
}

TEST_CASE("Root::setExpr")
{
    Root r;

    SECTION("Return value")
    {
        auto x = r.insertCell(Tree::ROOT_SHEET, "x", "1");
        REQUIRE(!r.setExpr(x, "1"));
        REQUIRE(r.setExpr(x, "2"));
    }

    SECTION("Re-evaluation on parent change")
    {
        auto x = r.insertCell(Tree::ROOT_SHEET, "x", "1");
        auto y = r.insertCell(Tree::ROOT_SHEET, "y", "(+ 1 (x))");

        r.setExpr(x, "10");
        auto v = r.getValue({{Tree::ROOT_INSTANCE}, y});
        REQUIRE(v.value != nullptr);
        REQUIRE(v.valid == true);
        REQUIRE(v.str == "11");
    }

    SECTION("Upstream becoming invalid")
    {
        auto x = r.insertCell(Tree::ROOT_SHEET, "x", "1");
        auto y = r.insertCell(Tree::ROOT_SHEET, "y", "(+ 1 (x))");

        r.setExpr(x, "this isn't a valid expression");
        auto v = r.getValue({{Tree::ROOT_INSTANCE}, y});
        REQUIRE(v.value != nullptr);
        REQUIRE(v.valid == false);
        REQUIRE(v.str == "Invalid lookup");
    }

    SECTION("Multiple evaluation paths")
    {
        auto x = r.insertCell(Tree::ROOT_SHEET, "x", "1");
        r.insertCell(Tree::ROOT_SHEET, "a", "(+ 2 (x))");
        auto y = r.insertCell(Tree::ROOT_SHEET, "y", "(+ (a) (x))");
        r.setExpr(x, "3");

        auto v = r.getValue({{Tree::ROOT_INSTANCE}, y});
        REQUIRE(v.value != nullptr);
        REQUIRE(v.valid == true);
        REQUIRE(v.str == "8");
    }

    SECTION("Cell becoming an output")
    {
        auto sum = r.insertSheet(Tree::ROOT_SHEET, "Sum");

        auto a = r.insertCell(sum, "a", "10");
        auto i = r.insertInstance(Tree::ROOT_SHEET, "i", sum);

        auto b = r.insertCell(Tree::ROOT_SHEET, "b", "(+ 1 (i 'a))");
        r.setExpr(a, "(output 5)");

        REQUIRE(r.getValue({{Tree::ROOT_INSTANCE}, b}).str == "6");
    }

    SECTION("Cell becoming an output (same value)")
    {
        auto sum = r.insertSheet(Tree::ROOT_SHEET, "Sum");

        auto a = r.insertCell(sum, "a", "10");
        auto i = r.insertInstance(Tree::ROOT_SHEET, "i", sum);

        auto b = r.insertCell(Tree::ROOT_SHEET, "b", "(+ 1 (i 'a))");
        r.setExpr(a, "(output 10)");

        REQUIRE(r.getValue({{Tree::ROOT_INSTANCE}, b}).str == "11");
    }
}

TEST_CASE("Root::canInsertSheet")
{
    Root r;
    REQUIRE(r.canInsertSheet(Tree::ROOT_SHEET, "Sum"));
    REQUIRE(!r.canInsertSheet(Tree::ROOT_SHEET, "sum"));
    REQUIRE(!r.canInsertSheet(Tree::ROOT_SHEET, ""));

    REQUIRE(r.canInsertSheet(Tree::ROOT_SHEET, "A"));
    REQUIRE(r.canInsertSheet(Tree::ROOT_SHEET, "B"));
    REQUIRE(r.canInsertSheet(Tree::ROOT_SHEET, "C"));

    REQUIRE(!r.canInsertSheet(Tree::ROOT_SHEET, ""));
    REQUIRE(!r.canInsertSheet(Tree::ROOT_SHEET, "a"));
    REQUIRE(!r.canInsertSheet(Tree::ROOT_SHEET, "b"));
    REQUIRE(!r.canInsertSheet(Tree::ROOT_SHEET, "c"));
    REQUIRE(!r.canInsertSheet(Tree::ROOT_SHEET, "12c"));
    REQUIRE(!r.canInsertSheet(Tree::ROOT_SHEET, "with a space"));

    r.insertSheet(Tree::ROOT_SHEET, "Sum");
    REQUIRE(!r.canInsertSheet(Tree::ROOT_SHEET, "Sum"));
}

TEST_CASE("Root::insertSheet")
{
    Root r;
    auto sum = r.insertSheet(Tree::ROOT_SHEET, "Sum");
    REQUIRE(sum.i == 2);

    auto mul = r.insertSheet(Tree::ROOT_SHEET, "Mul");
    REQUIRE(mul.i == 3);
}

TEST_CASE("Root::eraseSheet")
{
    Root r;
    auto sum = r.insertSheet(Tree::ROOT_SHEET, "Sum");

    auto i = r.insertInstance(Tree::ROOT_SHEET, "i", sum);
    REQUIRE(!r.checkItemName(Tree::ROOT_SHEET, "i"));

    r.eraseSheet(sum);
    REQUIRE(r.checkItemName(Tree::ROOT_SHEET, "i"));
}

TEST_CASE("Root::callSheet")
{
    Root r;
    auto sum = r.insertSheet(Tree::ROOT_SHEET, "Sum");
    auto c = r.insertCell(Tree::ROOT_SHEET, "a", "13.5");

    SECTION("No cells")
    {
        std::string err;
        auto out = r.callSheet({{Tree::ROOT_INSTANCE}, c}, sum, {}, &err);
        REQUIRE(err == "");
        REQUIRE(out.size() == 0);
    }

    SECTION("Too few inputs")
    {
        r.insertCell(sum, "in", "(input 12)");
        std::string err;
        auto out = r.callSheet({{Tree::ROOT_INSTANCE}, c}, sum, {}, &err);
        REQUIRE(err != "");
        REQUIRE(out.size() == 0);
    }

    SECTION("Too many inputs")
    {
        r.insertCell(sum, "in", "(input 12)");
        std::string err;
        auto v = Value(nullptr, "", true);
        auto out = r.callSheet({{Tree::ROOT_INSTANCE}, c}, sum, {v, v}, &err);
        REQUIRE(err != "");
        REQUIRE(out.size() == 0);
    }

    SECTION("Correct number of inputs")
    {
        r.insertCell(sum, "in", "(input 12)");

        // Get a value from c (this is just a way to get a value interpreter
        // ValuePtr without jumping through many hoops)
        auto val = r.getTree().at(c).cell()->values.at({Tree::ROOT_INSTANCE});

        std::string err;
        auto out = r.callSheet({{Tree::ROOT_INSTANCE}, c}, sum, {val}, &err);
        REQUIRE(err == "");
        REQUIRE(out.size() == 1);
        REQUIRE(out.count("in") == 1);
        REQUIRE(out.at("in").str == "13.5");
    }

    SECTION("Output value")
    {
        r.insertCell(sum, "in", "(input 12)");
        r.insertCell(sum, "out", "(output (+ (in) 15))");

        std::string err;
        auto val = r.getTree().at(c).cell()->values.at({Tree::ROOT_INSTANCE});
        auto out = r.callSheet({{Tree::ROOT_INSTANCE}, c}, sum, {val}, &err);

        REQUIRE(err == "");
        REQUIRE(out.size() == 2);
        REQUIRE(out.count("out") == 1);
        REQUIRE(out.at("out").str == "28.5");
    }
}

TEST_CASE("Root::insertInstance")
{
    Root r;

    auto sum = r.insertSheet(Tree::ROOT_SHEET, "Sum");

    SECTION("Cells within sheet")
    {
        auto a = r.insertCell(sum, "a", "1");
        auto b = r.insertCell(sum, "b", "2");
        auto out = r.insertCell(sum, "out", "(+ (a) (b))");

        auto i = r.insertInstance(Tree::ROOT_SHEET, "instance", sum);
        REQUIRE(r.getValue({{Tree::ROOT_INSTANCE, i}, out}).str == "3");
    }

    SECTION("Multiple instances")
    {
        auto a = r.insertCell(sum, "a", "1");
        auto b = r.insertCell(sum, "b", "2");
        auto out = r.insertCell(sum, "out", "(+ (a) (b))");

        auto i = r.insertInstance(Tree::ROOT_SHEET, "instance", sum);
        auto j = r.insertInstance(Tree::ROOT_SHEET, "jnstance", sum);
        REQUIRE(r.getValue({{Tree::ROOT_INSTANCE, i}, out}).value !=
                r.getValue({{Tree::ROOT_INSTANCE, j}, out}).value);
    }

    SECTION("Sheet input default")
    {
        auto a = r.insertCell(sum, "a", "(input 10)");
        auto b = r.insertCell(sum, "b", "2");
        auto out = r.insertCell(sum, "out", "(+ (a) (b))");

        auto i = r.insertInstance(Tree::ROOT_SHEET, "instance", sum);
        REQUIRE(r.getValue({{Tree::ROOT_INSTANCE, i}, out}).str == "12");
    }

    SECTION("Input evaluation environment")
    {
        auto input = r.insertCell(sum, "in", "(input (+ 1 (a)))");
        auto i = r.insertInstance(Tree::ROOT_SHEET, "instance", sum);

        auto a = r.insertCell(Tree::ROOT_SHEET, "a", "12");
        REQUIRE(r.getValue({{Tree::ROOT_INSTANCE, i}, input}).str == "13");

        r.setExpr(a, "13");
        REQUIRE(r.getValue({{Tree::ROOT_INSTANCE, i}, input}).str == "14");
    }

    SECTION("Output values")
    {
        auto a = r.insertCell(sum, "a", "(output 10)");
        auto i = r.insertInstance(Tree::ROOT_SHEET, "i", sum);

        auto b = r.insertCell(Tree::ROOT_SHEET, "b", "(+ 1 (i 'a))");
        REQUIRE(r.getValue({{Tree::ROOT_INSTANCE}, b}).str == "11");
    }

    SECTION("Detecting a new instance + output")
    {
        auto b = r.insertCell(Tree::ROOT_SHEET, "b", "(+ 1 (i 'a))");

        auto a = r.insertCell(sum, "a", "(output 10)");
        auto i = r.insertInstance(Tree::ROOT_SHEET, "i", sum);

        REQUIRE(r.getValue({{Tree::ROOT_INSTANCE}, b}).str == "11");
    }
}

TEST_CASE("Root::setInput")
{
    Root r;

    auto sum = r.insertSheet(Tree::ROOT_SHEET, "Sum");

    auto a = r.insertCell(sum, "a", "(input 10)");
    auto b = r.insertCell(sum, "b", "2");
    auto out = r.insertCell(sum, "out", "(+ (a) (b))");

    auto i = r.insertInstance(Tree::ROOT_SHEET, "instance", sum);
    r.setInput(i, a, "12");

    REQUIRE(r.getValue({{Tree::ROOT_INSTANCE, i}, out}).str == "14");
}

TEST_CASE("Root::eraseInstance")
{
    Root r;

    auto sum = r.insertSheet(Tree::ROOT_SHEET, "Sum");

    SECTION("Re-evaluating outputs")
    {
        auto b = r.insertCell(Tree::ROOT_SHEET, "b", "(+ 1 (i 'a))");

        auto a = r.insertCell(sum, "a", "(output 10)");
        auto i = r.insertInstance(Tree::ROOT_SHEET, "i", sum);

        r.eraseInstance(i);
        CAPTURE(r.getValue({{Tree::ROOT_INSTANCE}, b}).str);
        REQUIRE(r.getValue({{Tree::ROOT_INSTANCE}, b}).valid == false);
    }

    SECTION("Cleaning of values")
    {
        auto a = r.insertCell(sum, "a", " 12");
        REQUIRE(r.getTree().at(a).cell()->values.size() == 0);

        // Insert an instance; one value should be created
        auto i = r.insertInstance(0, "instance", sum);
        REQUIRE(r.getTree().at(a).cell()->values.size() == 1);

        // After erasing the instance, that values should be cleaned up
        r.eraseInstance(i);
        REQUIRE(r.getTree().at(a).cell()->values.size() == 0);
    }

    SECTION("Cleaning of dependency list")
    {
        auto input = r.insertCell(sum, "in", "(input (+ 1 (a)))");
        auto i = r.insertInstance(Tree::ROOT_SHEET, "instance", sum);

        auto a = r.insertCell(Tree::ROOT_SHEET, "a", "12"); // used in input
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
        auto a = r.insertCell(Tree::ROOT_SHEET, "a", "(+ 1 2)");
        r.clear();
        REQUIRE(true /* didn't crash */);
    }

    SECTION("With inputs")
    {
        auto sum = r.insertSheet(Tree::ROOT_SHEET, "Sum");
        auto input = r.insertCell(sum, "in", "(input (+ 1 (a)))");
        auto i = r.insertInstance(Tree::ROOT_SHEET, "instance", sum);

        auto a = r.insertCell(Tree::ROOT_SHEET, "a", "12"); // used in input
        r.clear();
        REQUIRE(true /* didn't crash */);
    }

    SECTION("Clear")
    {
        auto s = r.insertSheet(Tree::ROOT_SHEET, "Sheet");
        auto a = r.insertCell(s, "a", "15");

        r.clear();
        REQUIRE(r.insertCell(Tree::ROOT_SHEET, "a", "15").i == 2);
    }
}

TEST_CASE("Root::loadString")
{
    Root r;

    SECTION("Invalid strings")
    {
        REQUIRE(r.loadString("HI THERE") != "");
        REQUIRE(r.loadString("{}") != "");
        REQUIRE(r.loadString("{") != "");
    }

    SECTION("Value construction")
    {
        auto a = r.insertCell(Tree::ROOT_SHEET, "a", "15");

        auto before = r.getTree().toString();
        CAPTURE(before);
        REQUIRE(r.loadString(before) == "");
        REQUIRE(r.getValue({{Tree::ROOT_INSTANCE}, a}).str == "15");
    }
}

