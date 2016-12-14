#include "catch/catch.hpp"

#include "graph/root.hpp"

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

TEST_CASE("Root::eraseCell")
{
    Root r;

    SECTION("Basic erasing")
    {
        auto x = r.insertCell(0, "x", "(+ 1 2)");
        REQUIRE(!r.canInsertCell(0, "x"));
        r.eraseCell(x);
        REQUIRE(r.canInsertCell(0, "x"));
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
}
