#include "catch/catch.hpp"

#include "graph/symbol.hpp"

using namespace Graph;

TEST_CASE("SymbolTable::get (cell)")
{
    Root r;
    Dependencies deps;
    CellKey k(1, {});

    SymbolTable s(r, deps, k);
}
