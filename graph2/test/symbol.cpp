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

TEST_CASE("SymbolTable::todo")
{
    Root r;

    // Manually create cells and sheets
    auto s = r.sheets.insert(new Sheet());
    auto a = r.insert("a", new Cell(s));

    // Here's a cell that we'll look up
    auto b_ = new Cell(s);
    auto b = r.insert("b", b_);
    Interpreter::Value dummy = (Interpreter::Value)(new int(1));

    Dependencies deps;
    CellKey k(a, {0});
    SymbolTable t(r, deps, k);

    SECTION("Looking up a less-specialized value")
    {
        b_->values.insert({{}, Value(dummy, "omg", true)});

        auto v = t.get("b");
        REQUIRE(v.first == dummy);
        REQUIRE(v.second == SymbolTable::OKAY);
    }

    SECTION("Looking up an equally-specialized value")
    {
        b_->values.insert({{0}, Value(dummy, "omg", true)});

        auto v = t.get("b");
        REQUIRE(v.first == dummy);
        REQUIRE(v.second == SymbolTable::OKAY);
    }

    SECTION("Looking up an more-specialized value")
    {
        b_->values.insert({{0, 1}, Value(dummy, "omg", true)});

        auto v = t.get("b");
        REQUIRE(v.first == nullptr);
        REQUIRE(v.second == SymbolTable::MULTIPLE_VALUES);

        auto todo = t.todo;
        REQUIRE(std::distance(todo.first, todo.second) == 1);
        REQUIRE(todo.first->first == Cell::Env({0, 1}));
    }

    SECTION("Looking up multiple more-specialized values")
    {
        b_->values.insert({{0, 1}, Value(dummy, "omg", true)});
        b_->values.insert({{0, 2}, Value(dummy, "omg", true)});

        auto v = t.get("b");
        REQUIRE(v.first == nullptr);
        REQUIRE(v.second == SymbolTable::MULTIPLE_VALUES);

        auto todo = t.todo;
        REQUIRE(std::distance(todo.first, todo.second) == 2);
        REQUIRE(todo.first->first == Cell::Env({0, 1}));
        ++todo.first;
        REQUIRE(todo.first->first == Cell::Env({0, 2}));
    }
}
