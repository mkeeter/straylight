#include <catch/catch.hpp>

#include "s7/s7.h"
#include "kernel/bind/bind_s7.h"

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

s7_scheme* get_scm()
{
    s7_scheme* sc = s7_init();
    Kernel::Bind::init(sc);
    s7_set_current_error_port(sc, s7_open_output_string(sc));
    return sc;
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE("shape_add")
{
    auto sc = get_scm();

    REQUIRE(eval(sc, "(+)") == "0.0");
    REQUIRE(eval(sc, "(+ 1)") == "1.0");
    REQUIRE(eval(sc, "(+ 2)") == "2.0");
    REQUIRE(eval(sc, "(+ 1 2)") == "3.0");
    REQUIRE(eval(sc, "(+ 1 2 3 4)") == "10.0");
}

TEST_CASE("shape_mul")
{
    auto sc = get_scm();

    REQUIRE(eval(sc, "(*)") == "1.0");
    REQUIRE(eval(sc, "(* 1)") == "1.0");
    REQUIRE(eval(sc, "(* 2)") == "2.0");
    REQUIRE(eval(sc, "(* 1 2)") == "2.0");
    REQUIRE(eval(sc, "(* 1 2 3 4)") == "24.0");
}

TEST_CASE("shape_min")
{
    auto sc = get_scm();

    REQUIRE(eval(sc, "(min)") == "wrong-number-of-args");
    REQUIRE(eval(sc, "(min 1)") == "1.0");
    REQUIRE(eval(sc, "(min 2)") == "2.0");
    REQUIRE(eval(sc, "(min 1 2)") == "1.0");
    REQUIRE(eval(sc, "(min 1 2 3 4)") == "1.0");
}

TEST_CASE("shape_max")
{
    auto sc = get_scm();

    REQUIRE(eval(sc, "(max)") == "wrong-number-of-args");
    REQUIRE(eval(sc, "(max 1)") == "1.0");
    REQUIRE(eval(sc, "(max 2)") == "2.0");
    REQUIRE(eval(sc, "(max 1 2)") == "2.0");
    REQUIRE(eval(sc, "(max 1 2 3 4)") == "4.0");
}

TEST_CASE("shape_sub")
{
    auto sc = get_scm();

    REQUIRE(eval(sc, "(-)") == "wrong-number-of-args");
    REQUIRE(eval(sc, "(- 1)") == "-1.0");
    REQUIRE(eval(sc, "(- 2)") == "-2.0");
    REQUIRE(eval(sc, "(- 1 2)") == "-1.0");
    REQUIRE(eval(sc, "(- 1 2 3 4)") == "-8.0");
}

TEST_CASE("shape_div")
{
    auto sc = get_scm();

    REQUIRE(eval(sc, "(/)") == "wrong-number-of-args");
    REQUIRE(eval(sc, "(/ 1)") == "1.0");
    REQUIRE(eval(sc, "(/ 2)") == "0.5");
    REQUIRE(eval(sc, "(/ 1 4)") == "0.25");
    REQUIRE(eval(sc, "(/ 1 2 4)") == "0.125");
}

TEST_CASE("shape_atan")
{
    auto sc = get_scm();

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
    auto sc = get_scm();

    REQUIRE(eval(sc, "(expt)") == "wrong-number-of-args");
    REQUIRE(num(sc, "(expt 1 4)") == 1);
    REQUIRE(num(sc, "(expt 2 4)") == 16);
    REQUIRE(num(sc, "(expt 4 1/2)") == 2);

    REQUIRE(num(sc,
        "(let* ((f (lambda (x y z) x))"
        "       (d (expt (make-shape f) 1/3)))"
        "(d 27 0 0))") == 3);

    // TODO: more tests of tree here
}

TEST_CASE("shape_mod")
{
    auto sc = get_scm();

    REQUIRE(eval(sc, "(modulo)") == "wrong-number-of-args");
    REQUIRE(eval(sc, "(modulo 1)") == "wrong-number-of-args");
    REQUIRE(eval(sc, "(modulo 3 2)") == "1.0");
    REQUIRE(eval(sc, "(modulo -5 4)") == "3.0");
}

TEST_CASE("*cell-reader*")
{
    auto sc = get_scm();

    SECTION("Number -> Shape")
    {
        auto out = s7_eval_c_string(sc, "(eval (car (*cell-reader* '(12) '(1 2 3)))))");
        CAPTURE(s7_object_to_c_string(sc, out));
        REQUIRE(Kernel::Bind::is_shape(out));
    }

    SECTION("Same *env* produces same Tree")
    {
        auto a = s7_eval_c_string(sc, "(eval (car (*cell-reader* '(12) '(1 2 3)))))");
        auto b = s7_eval_c_string(sc, "(eval (car (*cell-reader* '(12) '(1 2 3)))))");
        REQUIRE(Kernel::Bind::get_shape(a)->tree ==
                Kernel::Bind::get_shape(b)->tree);
    }

    SECTION("Different *env* produces different Tree")
    {
        auto a = s7_eval_c_string(sc, "(eval (car (*cell-reader* '(12) '(1 2)))))");
        auto b = s7_eval_c_string(sc, "(eval (car (*cell-reader* '(12) '(1 3)))))");

        REQUIRE(!(Kernel::Bind::get_shape(a)->tree ==
                  Kernel::Bind::get_shape(b)->tree));
    }

    SECTION("Getting env back out")
    {
        auto a = s7_eval_c_string(sc, "(eval (car (*cell-reader* '(12) '(1 2 3)))))");

        auto env = Kernel::Bind::envOf(sc, Kernel::Bind::get_shape(a)->tree);
        REQUIRE(env.size() == 3);

        auto itr = env.begin();
        REQUIRE(*itr++ == 1);
        REQUIRE(*itr++ == 2);
        REQUIRE(*itr++ == 3);
    }
}
