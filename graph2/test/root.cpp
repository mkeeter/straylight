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
}
