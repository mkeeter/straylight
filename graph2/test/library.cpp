#include "catch/catch.hpp"

#include "graph/library.hpp"

TEST_CASE("Library::canInsert")
{
    Library lib;

    SECTION("Into empty library")
    {
        REQUIRE(lib.canInsert("a", 0));
        REQUIRE(lib.canInsert("b", 0));
        REQUIRE(lib.canInsert("c", 0));

        REQUIRE(lib.canInsert("", 0));
        REQUIRE(lib.canInsert("a", 1));
        REQUIRE(lib.canInsert("", 1));
    }

    SECTION("With other sheets")
    {
        lib.insert("a", 0);
        REQUIRE(!lib.canInsert("a", 0));
    }
}

TEST_CASE("Library::insert")
{
    Library lib;

    auto a = lib.insert("a", 0);
    REQUIRE(a.i == 1);

    auto b = lib.insert("b", 0);
    REQUIRE(b.i == 2);

    auto sub = lib.insert("c", a);
    REQUIRE(sub.i == 3);
}

TEST_CASE("Library::erase")
{
    Library lib;

    auto a = lib.insert("a", 0);
    REQUIRE(!lib.canInsert("a", 0));

    lib.erase(a);
    REQUIRE(lib.canInsert("a", 0));
}

TEST_CASE("Library::nameOf")
{
    Library lib;

    auto a = lib.insert("a", 0);
    REQUIRE(lib.nameOf(a) == "a");

    auto b = lib.insert("b", 0);
    REQUIRE(lib.nameOf(b) == "b");

    auto sub = lib.insert("c", a);
    REQUIRE(lib.nameOf(sub) == "c");
}

TEST_CASE("Library::rename")
{
    Library lib;

    auto a = lib.insert("a", 0);
    REQUIRE(lib.nameOf(a) == "a");

    lib.rename(a, "b");
    REQUIRE(lib.nameOf(a) == "b");

    REQUIRE(!lib.canInsert("b", 0));
}

TEST_CASE("Library::operator[]")
{
    Library lib;

    auto a = lib.insert("a", 0);
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

TEST_CASE("Library::iterSheets")
{
    Library lib;

    auto a = lib.insert("a", 0);
    auto b = lib.insert("b", 0);
    auto c = lib.insert("c", 0);
    auto d = lib.insert("d", 1);

    SECTION("Following alphabetical order")
    {
        auto abc = lib.iterSheets(0);
        REQUIRE(abc.size() == 3);
        REQUIRE(abc.front() == a);
        abc.pop_front();
        REQUIRE(abc.front() == b);
        abc.pop_front();
        REQUIRE(abc.front() == c);
    }

    SECTION("Inside nested sheet")
    {
        auto d_ = lib.iterSheets(1);
        REQUIRE(d_.size() == 1);
        REQUIRE(d_.front() == d);
    }

    SECTION("After renaming")
    {
        lib.rename(a, "e");

        auto bce = lib.iterSheets(0);
        REQUIRE(bce.size() == 3);
        REQUIRE(bce.front() == b);
        bce.pop_front();
        REQUIRE(bce.front() == c);
        bce.pop_front();
        REQUIRE(bce.front() == a);
    }
}
