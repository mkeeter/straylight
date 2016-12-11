#include "catch/catch.hpp"

#include "graph/interpreter.hpp"
#include "graph/root.hpp"

TEST_CASE("Interpreter::cellType")
{
    Root r;
    Dependencies d(r);
    auto interp = Interpreter(r, &d);

    REQUIRE(interp.cellType("(input 12)") == Cell::INPUT);
    REQUIRE(interp.cellType("(input (+ 1 2))") == Cell::INPUT);
    REQUIRE(interp.cellType("(output 12)") == Cell::OUTPUT);
    REQUIRE(interp.cellType("(output (+ 1 2))") == Cell::OUTPUT);
    REQUIRE(interp.cellType("(+ 1 2)") == Cell::BASIC);
    REQUIRE(interp.cellType("\"omg") == Cell::BASIC);
    REQUIRE(interp.cellType("'input") == Cell::BASIC);
}

TEST_CASE("Interpreter::defaultExpr")
{
    Root r;
    Dependencies d(r);
    auto interp = Interpreter(r, &d);

    REQUIRE(interp.defaultExpr("(input 12)") == "12");
    REQUIRE(interp.defaultExpr("(input (+ 1 2))") == "(+ 1 2)");
}

TEST_CASE("Interpreter::eval")
{
    Root r;
    Dependencies d(r);
    auto lock = r.Lock();

    auto interp = Interpreter(r, &d);
    auto cell = r.insertCell(0, "c", "");
    const auto& value = r.getItem(cell).cell()->values.at({0});
    CellKey key = {{0}, cell};

    SECTION("Basic eval")
    {
        r.setExpr(cell, "12");
        interp.eval(key);
        REQUIRE(value.str == "12");
    }

    SECTION("Changing value")
    {
        r.setExpr(cell, "12");
        interp.eval(key);

        r.setExpr(cell, "13");

        // On the first evaluation, the value changes
        REQUIRE(interp.eval(key).value != nullptr);
        // On the second evaluation, the value stays the same
        REQUIRE(interp.eval(key).value == nullptr);
        REQUIRE(value.str == "13");
    }

    SECTION("Changing error")
    {
        r.setExpr(cell, "omg");
        interp.eval(key);
        r.setExpr(cell, "wtf");

        // On the first evaluation, the error changes
        REQUIRE(interp.eval(key).value != nullptr);
        // On the second evaluation, the error stays the same
        REQUIRE(interp.eval(key).value == nullptr);
    }

    SECTION("Input at top level")
    {
        r.setExpr(cell, "omg");
        interp.eval(key);

        REQUIRE(value.str == "Input at top level");
        REQUIRE(value.valid == false);
    }

    SECTION("Multiple clauses")
    {
        r.setExpr(cell, "(define (f x) (+ 1 x))"
                        "(f 3)");
        interp.eval(key);

        REQUIRE(value.str == "4");
        REQUIRE(value.valid == true);
    }

    SECTION("output")
    {
        r.setExpr(cell, "(output 12)");
        interp.eval(key);

        REQUIRE(value.str == "12");
        REQUIRE(value.valid == true);
    }

    SECTION("output with internal clauses")
    {
        r.setExpr(cell, "(output (define (f x) (+ 1 x)) (f 12))");
        interp.eval(key);

        REQUIRE(value.str == "13");
        REQUIRE(value.valid == true);
    }

    SECTION("output with external clauses (invalid)")
    {
        r.setExpr(cell, "(output 12)(+ 1 2)");
        interp.eval(key);

        REQUIRE(value.str == "Output must only have one clause");
        REQUIRE(value.valid == false);
    }
}
