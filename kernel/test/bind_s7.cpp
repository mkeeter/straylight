#include <catch/catch.hpp>

#include "s7/s7.h"
#include "kernel/bind/bind_s7.h"

#define EVAL(s) s7_object_to_c_string(sc, s7_eval_c_string(sc, s))

std::string eval(s7_scheme* sc, std::string expr)
{
    auto out = s7_object_to_c_string(sc, s7_eval_c_string(sc, expr.c_str()));
    std::string s(out);
    free(out);
    return s;
}

double num(s7_scheme* sc, std::string expr)
{
    auto out = s7_eval_c_string(sc, expr.c_str());
    assert(s7_is_number(out));
    return s7_real(out);
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE("shape_add")
{
    s7_scheme* sc = s7_init();
    kernel_bind_s7(sc);

    REQUIRE(eval(sc, "(+)") == "0.0");
    REQUIRE(eval(sc, "(+ 1)") == "1.0");
    REQUIRE(eval(sc, "(+ 2)") == "2.0");
    REQUIRE(eval(sc, "(+ 1 2)") == "3.0");
    REQUIRE(eval(sc, "(+ 1 2 3 4)") == "10.0");
}

TEST_CASE("shape_mul")
{
    s7_scheme* sc = s7_init();
    kernel_bind_s7(sc);

    REQUIRE(eval(sc, "(*)") == "1.0");
    REQUIRE(eval(sc, "(* 1)") == "1.0");
    REQUIRE(eval(sc, "(* 2)") == "2.0");
    REQUIRE(eval(sc, "(* 1 2)") == "2.0");
    REQUIRE(eval(sc, "(* 1 2 3 4)") == "24.0");
}

TEST_CASE("shape_min")
{
    s7_scheme* sc = s7_init();
    kernel_bind_s7(sc);

    REQUIRE(eval(sc, "(min)") == "wrong-number-of-args");
    REQUIRE(eval(sc, "(min 1)") == "1.0");
    REQUIRE(eval(sc, "(min 2)") == "2.0");
    REQUIRE(eval(sc, "(min 1 2)") == "1.0");
    REQUIRE(eval(sc, "(min 1 2 3 4)") == "1.0");
}

TEST_CASE("shape_max")
{
    s7_scheme* sc = s7_init();
    kernel_bind_s7(sc);

    REQUIRE(eval(sc, "(max)") == "wrong-number-of-args");
    REQUIRE(eval(sc, "(max 1)") == "1.0");
    REQUIRE(eval(sc, "(max 2)") == "2.0");
    REQUIRE(eval(sc, "(max 1 2)") == "2.0");
    REQUIRE(eval(sc, "(max 1 2 3 4)") == "4.0");
}

TEST_CASE("shape_sub")
{
    s7_scheme* sc = s7_init();
    kernel_bind_s7(sc);

    REQUIRE(eval(sc, "(-)") == "wrong-number-of-args");
    REQUIRE(eval(sc, "(- 1)") == "-1.0");
    REQUIRE(eval(sc, "(- 2)") == "-2.0");
    REQUIRE(eval(sc, "(- 1 2)") == "-1.0");
    REQUIRE(eval(sc, "(- 1 2 3 4)") == "-8.0");
}

TEST_CASE("shape_div")
{
    s7_scheme* sc = s7_init();
    kernel_bind_s7(sc);

    REQUIRE(eval(sc, "(/)") == "wrong-number-of-args");
    REQUIRE(eval(sc, "(/ 1)") == "1.0");
    REQUIRE(eval(sc, "(/ 2)") == "0.5");
    REQUIRE(eval(sc, "(/ 1 4)") == "0.25");
    REQUIRE(eval(sc, "(/ 1 2 4)") == "0.125");
}

TEST_CASE("shape_atan")
{
    s7_scheme* sc = s7_init();
    kernel_bind_s7(sc);

    REQUIRE(eval(sc, "(atan)") == "wrong-number-of-args");
    REQUIRE(num(sc, "(atan 1)") == Approx(M_PI/4));
    REQUIRE(num(sc, "(atan 2)") == Approx(1.10715));

    // Check all four quadrants of atan2
    REQUIRE(num(sc, "(atan 1 4)") == Approx(0.244978));
    REQUIRE(num(sc, "(atan -1 4)") == Approx(-0.244978));
    REQUIRE(num(sc, "(atan 1 -4)") == Approx(2.89661));
    REQUIRE(num(sc, "(atan -1 -4)") == Approx(-2.89661));
}

TEST_CASE("shape_expt")
{
    s7_scheme* sc = s7_init();
    kernel_bind_s7(sc);

    REQUIRE(eval(sc, "(expt)") == "wrong-number-of-args");
    REQUIRE(num(sc, "(expt 1 4)") == 1);
    REQUIRE(num(sc, "(expt 2 4)") == 16);
    REQUIRE(num(sc, "(expt 4 1/2)") == 2);

    // TODO: more tests of tree here
}