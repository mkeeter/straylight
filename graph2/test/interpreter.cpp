#include "catch/catch.hpp"

#include "graph/interpreter.hpp"

TEST_CASE("Interpreter::cellType")
{
    auto interp = Interpreter();

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
    auto interp = Interpreter();

    REQUIRE(interp.defaultExpr("(input 12)") == "12");
    REQUIRE(interp.defaultExpr("(input (+ 1 2))") == "(+ 1 2)");
}

