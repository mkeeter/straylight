#include "catch/catch.hpp"

#include "graph/symbol.hpp"

using namespace Graph;

TEST_CASE("SymbolTable::get (cell)")
{
    Root r;

    // Manually create cells and sheets
    auto s = r.sheets.insert(new Sheet());
    auto a = r.insert("a", new Cell(s));

    // Here's a cell that we'll look up
    auto b_ = new Cell(s);
    auto b = r.insert("b", b_);

    Dependencies deps;
    CellKey k(a, {});

    SymbolTable t(r, deps, k);

    auto va = t.get("a");
    REQUIRE(va.first == nullptr);
    REQUIRE(va.second == SymbolTable::RECURSIVE);

    auto vc = t.get("c");
    REQUIRE(vc.first == nullptr);
    REQUIRE(vc.second == SymbolTable::NO_SUCH_NAME);

    auto vb = t.get("b");
    REQUIRE(vb.first == nullptr);
    REQUIRE(vb.second == SymbolTable::MISSING_ENV);

    Interpreter::Value dummy = (Interpreter::Value)(new int(1));
    b_->values.insert({{}, Value(dummy, "omg", true)});
    auto vb_ = t.get("b");
    REQUIRE(vb_.first == dummy);
    REQUIRE(vb_.second == SymbolTable::OKAY);
}
