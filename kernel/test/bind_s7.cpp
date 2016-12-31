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

TEST_CASE("Overloaded addition")
{
    s7_scheme* sc = s7_init();
    kernel_bind_s7(sc);

    REQUIRE(eval(sc, "(+)") == "0.0");
    REQUIRE(eval(sc, "(+ 1)") == "1.0");
    REQUIRE(eval(sc, "(+ 2)") == "2.0");
    REQUIRE(eval(sc, "(+ 1 2)") == "3.0");
    REQUIRE(eval(sc, "(+ 1 2 3 4)") == "10.0");
}
