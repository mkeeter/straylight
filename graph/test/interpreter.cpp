#include "catch/catch.hpp"

#include "graph/interpreter.hpp"
#include "graph/root.hpp"

#include "s7/s7.h"

using namespace Graph;

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
    auto cell = r.insertCell(Tree::ROOT_SHEET, "c", "");
    auto c = r.getTree().at(cell).cell();
    CellKey key = {{Tree::ROOT_INSTANCE}, cell};

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

TEST_CASE("Interpreter::isReserved")
{
    Root r;
    Dependencies d(r);
    auto interp = Interpreter(r, &d);

    REQUIRE(interp.isReserved("input"));
    REQUIRE(interp.isReserved("output"));
    REQUIRE(interp.isReserved("define"));
    REQUIRE(interp.isReserved("#f"));
    REQUIRE(interp.isReserved("+"));
    REQUIRE(interp.isReserved("#t"));
    REQUIRE(!interp.isReserved("hi"));
    REQUIRE(!interp.isReserved("omg"));
    REQUIRE(!interp.isReserved("d"));
    REQUIRE(!interp.isReserved("e"));
    REQUIRE(!interp.isReserved("i"));
}

TEST_CASE("Interpreter::keywords")
{
    Root r;
    Dependencies d(r);
    auto interp = Interpreter(r, &d);

    auto k = interp.keywords();

    REQUIRE(k.count("#t"));
    REQUIRE(k.count("define"));
    REQUIRE(k.count("lambda"));
    REQUIRE(!k.count("x"));
    REQUIRE(!k.count("y"));
    REQUIRE(!k.count("d"));
}

static s7_cell* customReader(s7_scheme* sc)
{
    return s7_eval_c_string(sc, "(lambda (sexp r c) '(123))");
}

TEST_CASE("Interpreter::setReader")
{
    Root r;
    Dependencies d(r);
    auto interp = Interpreter(r, &d);

    auto cell = r.insertCell(Tree::ROOT_SHEET, "c", "");
    auto c = r.getTree().at(cell).cell();
    CellKey key = {{Tree::ROOT_INSTANCE}, cell};

    // Install a custom reader that turns anything into 123
    interp.setReader(interp.call(customReader));

    r.setExpr(cell, "1");
    auto value = interp.eval(key);
    REQUIRE(value.str == "123");
}
