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

TEST_CASE("Dependencies::clear")
{
    // TODO
}

TEST_CASE("Dependencies::erase")
{
}
