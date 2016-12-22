#include "catch/catch.hpp"

#include "graph/library.hpp"

TEST_CASE("Library::canInsert")
{
    Library lib;

    SECTION("Into empty library")
    {
        REQUIRE(lib.canInsert(0, "a"));
        REQUIRE(lib.canInsert(0, "b"));
        REQUIRE(lib.canInsert(0, "c"));

        REQUIRE(!lib.canInsert(0, ""));
    }

    SECTION("With other sheets")
    {
        lib.insert(0, "a");
        REQUIRE(!lib.canInsert(0, "a"));
    }
}

TEST_CASE("Library::insert")
{
    Library lib;

    auto a = lib.insert(0, "a");
    REQUIRE(a.i == 1);

    auto b = lib.insert(0, "b");
    REQUIRE(b.i == 2);

    auto sub = lib.insert(a, "c");
    REQUIRE(sub.i == 3);
}

TEST_CASE("Library::erase")
{
    Library lib;

    auto a = lib.insert(0, "a");
    REQUIRE(!lib.canInsert(0, "a"));

    lib.erase(a);
    REQUIRE(lib.canInsert(0, "a"));
}

TEST_CASE("Library::nameOf")
{
    Library lib;

    auto a = lib.insert(0, "a");
    REQUIRE(lib.nameOf(a) == "a");

    auto b = lib.insert(0, "b");
    REQUIRE(lib.nameOf(b) == "b");

    auto sub = lib.insert(a, "c");
    REQUIRE(lib.nameOf(sub) == "c");
}

TEST_CASE("Library::rename")
{
    Library lib;

    auto a = lib.insert(0, "a");
    REQUIRE(lib.nameOf(a) == "a");

    lib.rename(a, "b");
    REQUIRE(lib.nameOf(a) == "b");

    REQUIRE(!lib.canInsert(0, "b"));
}

TEST_CASE("Library::at")
{
    Library lib;

    auto a = lib.insert(0, "a");
    const Sheet& s = lib.at(a);

    bool threw = false;
    try
    {
        const Sheet& s = lib.at(2);
    }
    catch (const std::out_of_range&)
    {
        threw = true;
    }
    REQUIRE(threw);
}

TEST_CASE("Library::childrenOf")
{
    Library lib;

    auto a = lib.insert(0, "a");
    auto b = lib.insert(0, "b");
    auto c = lib.insert(0, "c");
    auto d = lib.insert(1, "d");

    SECTION("Following alphabetical order")
    {
        auto abc = lib.childrenOf(0);
        REQUIRE(abc.size() == 3);
        REQUIRE(abc.front() == a);
        abc.pop_front();
        REQUIRE(abc.front() == b);
        abc.pop_front();
        REQUIRE(abc.front() == c);
    }

    SECTION("Inside nested sheet")
    {
        auto d_ = lib.childrenOf(1);
        REQUIRE(d_.size() == 1);
        REQUIRE(d_.front() == d);
    }

    SECTION("After renaming")
    {
        lib.rename(a, "e");

        auto bce = lib.childrenOf(0);
        REQUIRE(bce.size() == 3);
        REQUIRE(bce.front() == b);
        bce.pop_front();
        REQUIRE(bce.front() == c);
        bce.pop_front();
        REQUIRE(bce.front() == a);
    }
}
