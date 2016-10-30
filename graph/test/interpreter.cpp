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

    SECTION("Interpreter::isInput")
    {
        REQUIRE(interp.isInput(&i));
        REQUIRE(interp.isInput(&j));
        REQUIRE(!interp.isInput(&o));
        REQUIRE(!interp.isInput(&p));
        REQUIRE(!interp.isInput(&x));
        REQUIRE(!interp.isInput(&y));
        REQUIRE(!interp.isInput(&z));
    }

    SECTION("Interpreter::isOutput")
    {
        REQUIRE(interp.isOutput(&o));
        REQUIRE(interp.isOutput(&p));
        REQUIRE(!interp.isOutput(&i));
        REQUIRE(!interp.isOutput(&j));
        REQUIRE(!interp.isOutput(&x));
        REQUIRE(!interp.isOutput(&y));
        REQUIRE(!interp.isOutput(&z));
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
    Graph::Instance instance(&sheet);
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
}
