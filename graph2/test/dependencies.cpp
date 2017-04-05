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

    NameKey k = {0, "hi"};
    REQUIRE(toSet<CellKey>(d.inverse(k)).size() == 0);

    d.insert({3, {}}, k);
    REQUIRE(toSet<CellKey>(d.inverse(k)).size() == 1);

    d.insert({4, {}}, k);
    REQUIRE(toSet<CellKey>(d.inverse(k)).size() == 2);
}

TEST_CASE("Dependencies::insert(CellKey, CellKey)")
{
    Dependencies d;

    CellKey a = {0,{}};
    CellKey b = {1,{}};
    CellKey c = {2,{}};

    SECTION("Basics")
    {
        d.insert(a, b);
        d.insert(a, c);

        auto as = toSet<CellKey>(d.forward(a));
        REQUIRE(as.size() == 2);
        REQUIRE(as.count(b) == 1);
        REQUIRE(as.count(c) == 1);
        REQUIRE(toSet<CellKey>(d.inverse(a)).size() == 0);

        auto bs = toSet<CellKey>(d.inverse(b));
        REQUIRE(bs.size() == 1);
        REQUIRE(bs.count(a) == 1);
        REQUIRE(toSet<CellKey>(d.forward(b)).size() == 0);

        auto cs = toSet<CellKey>(d.inverse(c));
        REQUIRE(cs.size() == 1);
        REQUIRE(cs.count(a) == 1);
        REQUIRE(toSet<CellKey>(d.forward(c)).size() == 0);
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
    Dependencies d;

    CellKey a = {0,{}};
    CellKey b = {1,{}};
    CellKey c = {2,{}};

    SECTION("Breaking multi-element loop")
    {
        REQUIRE(d.insert(a, b) == false);
        REQUIRE(d.insert(b, c) == false);
        d.clear();
        REQUIRE(d.insert(c, a) == false);
    }
}

TEST_CASE("Dependencies::erase")
{
    Dependencies d;

    CellKey a = {0,{}};
    CellKey b = {1,{}};
    CellKey c = {2,{}};

    SECTION("Breaking multi-element loop")
    {
        REQUIRE(d.insert(a, b) == false);
        REQUIRE(d.insert(b, c) == false);
        d.erase(a);
        REQUIRE(d.insert(c, a) == false);
    }
}

TEST_CASE("Dependencies::find")
{
    Dependencies d;

    CellKey a  = {0,{}};
    CellKey a_ = {0,{1}};
    CellKey q  = {5,{1}};
    CellKey b  = {1,{}};
    CellKey c  = {2,{}};

    d.insert(b, a);
    d.insert(c, a_);
    d.insert(b, q);
    d.insert(c, q);

    auto itr = d.find(a);
    REQUIRE(itr->second == b);

    REQUIRE(++itr != d.end());
    REQUIRE(itr->first.specializes(a));
    REQUIRE(itr->second == c);

    REQUIRE(!(++itr)->first.specializes(a));
}
