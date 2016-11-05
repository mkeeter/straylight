#include "catch.hpp"

#include "cell.hpp"
#include "dependencies.hpp"
#include "interpreter.hpp"
#include "instance.hpp"
#include "sheet.hpp"

TEST_CASE("Interpreter parsing of expressions")
{
    auto interp = Graph::Interpreter();
    auto i = Graph::Cell("(input 12)", nullptr);
    auto j = Graph::Cell("(input (+ 1 2))", nullptr);
    auto o = Graph::Cell("(output 12)", nullptr);
    auto p = Graph::Cell("(output (+ 1 2))", nullptr);
    auto x = Graph::Cell("(+ 1 2)", nullptr);
    auto y = Graph::Cell("\"omg", nullptr);
    auto z = Graph::Cell("'input", nullptr);

    SECTION("Interpreter::cellType")
    {
        REQUIRE(interp.cellType(&i) == Graph::Cell::INPUT);
        REQUIRE(interp.cellType(&j) == Graph::Cell::INPUT);
        REQUIRE(interp.cellType(&o) == Graph::Cell::OUTPUT);
        REQUIRE(interp.cellType(&p) == Graph::Cell::OUTPUT);
        REQUIRE(interp.cellType(&x) == Graph::Cell::BASIC);
        REQUIRE(interp.cellType(&y) == Graph::Cell::BASIC);
        REQUIRE(interp.cellType(&z) == Graph::Cell::BASIC);
    }

    SECTION("Interpreter::defaultExpr")
    {
        REQUIRE(interp.defaultExpr(&i) == "12");
        REQUIRE(interp.defaultExpr(&j) == "(+ 1 2)");
    }
}

TEST_CASE("Interpreter::eval")
{
    Graph::Sheet sheet(nullptr);
    Graph::Instance instance(&sheet, nullptr);
    Graph::Cell cell("", &sheet);
    Graph::CellKey key = {{&instance}, &cell};
    Graph::Dependencies deps;

    Graph::Interpreter interp;
    SECTION("Basic eval")
    {
        cell.expr = "12";
        interp.eval(key, &deps);
        REQUIRE(cell.values[{&instance}].str == "12");
    }

    SECTION("Changing value")
    {
        cell.expr = "12";
        interp.eval(key, &deps);
        cell.expr = "13";

        // On the first evaluation, the value changes
        REQUIRE(interp.eval(key, &deps));
        // On the second evaluation, the value stays the same
        REQUIRE(!interp.eval(key, &deps));
        REQUIRE(cell.values[{&instance}].str == "13");
    }

    SECTION("Changing error")
    {
        cell.expr = "omg";
        interp.eval(key, &deps);
        cell.expr = "wtf";

        // On the first evaluation, the error changes
        REQUIRE(interp.eval(key, &deps));
        // On the second evaluation, the error stays the same
        REQUIRE(!interp.eval(key, &deps));
    }

    SECTION("Input at top level")
    {
        cell.expr = "(input 0)";
        interp.eval(key, &deps);
        REQUIRE(cell.values[key.first].str == "Input at top level");
        REQUIRE(cell.values[key.first].valid == false);
    }
}
