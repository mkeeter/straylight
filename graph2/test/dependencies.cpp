#include <iostream>
#include "catch/catch.hpp"

#include "graph/dependencies.hpp"

using namespace Graph;
using namespace boost;

template<class O, class T>
std::set<O> toSet(std::pair<T, T> itrs)
{
    std::set<O> out;
    for (auto i = itrs.first; i != itrs.second; ++i)
    {
        out.insert(i->second);
    }
    return out;
}

TEST_CASE("Dependencies::insert(CellKey, NameKey)")
{
    Dependencies d;

    Dependencies::NameKey k = {0, "hi"};
    REQUIRE(toSet<Dependencies::CellKey>(d.inverse(k)).size() == 0);

    d.insert({3, {}}, k);
    REQUIRE(toSet<Dependencies::CellKey>(d.inverse(k)).size() == 1);

    d.insert({4, {}}, k);
    REQUIRE(toSet<Dependencies::CellKey>(d.inverse(k)).size() == 2);
}

TEST_CASE("Dependencies::insert(CellKey, CellKey)")
{
    Dependencies d;

    Dependencies::CellKey a = {0,{}};
    Dependencies::CellKey b = {1,{}};
    Dependencies::CellKey c = {2,{}};

    SECTION("Basics")
    {
        d.insert(a, b);
        d.insert(a, c);

        auto as = toSet<Dependencies::CellKey>(d.forward(a));
        REQUIRE(as.size() == 2);
        REQUIRE(as.count(b) == 1);
        REQUIRE(as.count(c) == 1);
        REQUIRE(toSet<Dependencies::CellKey>(d.inverse(a)).size() == 0);

        auto bs = toSet<Dependencies::CellKey>(d.inverse(b));
        REQUIRE(bs.size() == 1);
        REQUIRE(bs.count(a) == 1);
        REQUIRE(toSet<Dependencies::CellKey>(d.forward(b)).size() == 0);

        auto cs = toSet<Dependencies::CellKey>(d.inverse(c));
        REQUIRE(cs.size() == 1);
        REQUIRE(cs.count(a) == 1);
        REQUIRE(toSet<Dependencies::CellKey>(d.forward(c)).size() == 0);
    }

    SECTION("Simple loop")
    {
        REQUIRE(d.insert(a, a) == true);
    }

    SECTION("Two-element loop")
    {
        REQUIRE(d.insert(a, b) == false);
        REQUIRE(d.insert(b, a) == true);
    }

    SECTION("Multi-element loop")
    {
        REQUIRE(d.insert(a, b) == false);
        REQUIRE(d.insert(b, c) == false);
        REQUIRE(d.insert(c, a) == true);
    }
}

TEST_CASE("Dependencies::clear")
{
    // TODO
}

TEST_CASE("Dependencies::erase")
{
}
