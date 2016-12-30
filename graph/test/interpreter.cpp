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
    auto c = r.getItem(cell).cell();
    CellKey key = {{0}, cell};

    SECTION("Basic eval")
    {
        r.setExpr(cell, "12");
        auto value = interp.eval(key);
        REQUIRE(value.str == "12");
    }

    SECTION("Changing value")
    {
        r.setExpr(cell, "12");
        r.setValue(key, interp.eval(key));

        // On the first evaluation, the value changes
        r.setExpr(cell, "13");
        auto v = interp.eval(key);
        REQUIRE(v.value != nullptr);
        r.setValue(key, v);

        // On the second evaluation, the value stays the same
        REQUIRE(interp.eval(key).value == nullptr);
    }

    SECTION("Changing error")
    {
        r.setExpr(cell, "omg"); // invalid script
        r.setValue(key, interp.eval(key));

        // On the first evaluation, the value changes
        r.setExpr(cell, "wtf"); // also invalid script
        auto v = interp.eval(key);
        REQUIRE(v.value != nullptr);
        r.setValue(key, v);

        // On the second evaluation, the value stays the same
        REQUIRE(interp.eval(key).value == nullptr);
    }

    SECTION("Input at top level")
    {
        r.setExpr(cell, "(input 12)");
        auto value = interp.eval(key);

        REQUIRE(value.str == "Input at top level");
        REQUIRE(value.valid == false);
    }

    SECTION("Multiple clauses")
    {
        r.setExpr(cell, "(define (f x) (+ 1 x))"
                        "(f 3)");
        auto value = interp.eval(key);

        REQUIRE(value.str == "4");
        REQUIRE(value.valid == true);
    }

    SECTION("output")
    {
        r.setExpr(cell, "(output 12)");
        auto value = interp.eval(key);

        REQUIRE(value.str == "12");
        REQUIRE(value.valid == true);
    }

    SECTION("output with internal clauses")
    {
        r.setExpr(cell, "(output (define (f x) (+ 1 x)) (f 12))");
        auto value = interp.eval(key);

        REQUIRE(value.str == "13");
        REQUIRE(value.valid == true);
    }

    SECTION("output with external clauses (invalid)")
    {
        r.setExpr(cell, "(output 12)(+ 1 2)");
        auto value = interp.eval(key);

        REQUIRE(value.str == "Output must only have one clause");
        REQUIRE(value.valid == false);
    }
}

TEST_CASE("Interpreter::isKeyword")
{
    Root r;
    Dependencies d(r);
    auto interp = Interpreter(r, &d);

    REQUIRE(interp.isKeyword("input"));
    REQUIRE(interp.isKeyword("output"));
    REQUIRE(interp.isKeyword("define"));
    REQUIRE(interp.isKeyword("#f"));
    REQUIRE(interp.isKeyword("+"));
    REQUIRE(interp.isKeyword("#t"));
    REQUIRE(!interp.isKeyword("hi"));
    REQUIRE(!interp.isKeyword("omg"));
}
