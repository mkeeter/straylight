#include "catch/catch.hpp"

#include "graph/id.hpp"

using namespace Graph;

struct Dummy_ {};
typedef Id<Dummy_> TestId;

TEST_CASE("IdMap::insert")
{
    IdMap<TestId, int, 3, 2> m;

    REQUIRE(m.insert(new int(1)).i == 3);
    REQUIRE(m.insert(new int(1)).i == 7);
    REQUIRE(m.insert(new int(1)).i == 11);

    IdMap<TestId, int, 2, 2> n;

    REQUIRE(n.insert(new int(1)).i == 2);
    REQUIRE(n.insert(new int(1)).i == 6);
    REQUIRE(n.insert(new int(1)).i == 10);


    IdMap<TestId, int, 0, 1> a;
    REQUIRE(a.insert(new int(1)).i == 0);
    REQUIRE(a.insert(new int(1)).i == 2);
    REQUIRE(a.insert(new int(1)).i == 4);
}
