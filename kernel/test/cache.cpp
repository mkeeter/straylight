#include <catch/catch.hpp>

#include "kernel/tree/cache.hpp"

using namespace Kernel;

TEST_CASE("Constructing a simple shape")
{
    Cache::reset();
    auto t = Cache::instance();

    Cache::Id out = t->operation(Opcode::ADD, t->X(),
            t->constant(1));
    REQUIRE(out == 3);
}

TEST_CASE("Deduplication of variables")
{
    Cache::reset();
    auto t = Cache::instance();

    Cache::Id xa = t->X();
    Cache::Id xb = t->X();
    REQUIRE(xa == xb);

    Cache::Id ya = t->Y();
    REQUIRE(xa != ya);
}

TEST_CASE("Deduplication of constants")
{
    Cache::reset();
    auto t = Cache::instance();

    Cache::Id ca = t->constant(3.14);
    Cache::Id cb = t->constant(3.14);
    REQUIRE(ca == cb);

    Cache::Id cc = t->constant(4);
    REQUIRE(ca != cc);
}

TEST_CASE("Deduplication of operations")
{
    Cache::reset();
    auto t = Cache::instance();

    Cache::Id oa = t->operation(Opcode::ADD, t->X(), t->constant(1));
    Cache::Id ob = t->operation(Opcode::ADD, t->X(), t->constant(1));
    REQUIRE(oa == ob);

    Cache::Id oc = t->operation(Opcode::ADD, t->X(), t->constant(2));
    REQUIRE(oa != oc);
}

TEST_CASE("Found flag propagation")
{
    Cache::reset();
    auto t = Cache::instance();

    Cache::Id oa = t->operation(Opcode::ADD, t->X(), t->constant(1));
    Cache::Id ob = t->operation(Opcode::MUL, t->Y(), t->constant(1));

    auto f = t->findConnected(oa);

    REQUIRE(f.count(oa));
    REQUIRE(f.count(t->X()));
    REQUIRE(f.count(t->constant(1)));

    REQUIRE(!f.count(ob));
    REQUIRE(!f.count(t->Y()));
}

TEST_CASE("Collapsing of identites")
{
    Cache::reset();
    auto t = Cache::instance();

    SECTION("Addition")
    {
        Cache::Id oa = t->operation(Opcode::ADD, t->X(), t->constant(0));
        REQUIRE(oa == t->X());

        Cache::Id ob = t->operation(Opcode::ADD, t->constant(0), t->X());
        REQUIRE(ob == t->X());
    }

    SECTION("Subtraction")
    {
        Cache::Id oa = t->operation(Opcode::SUB, t->X(), t->constant(0));
        REQUIRE(oa == t->X());

        Cache::Id ob = t->operation(Opcode::SUB, t->constant(0), t->X());
        REQUIRE(t->opcode(ob) == Opcode::NEG);
        REQUIRE(t->lhs(ob) == t->X());
    }

    SECTION("Multiplication")
    {
        Cache::Id oa = t->operation(Opcode::MUL, t->X(), t->constant(1));
        REQUIRE(oa == t->X());

        Cache::Id ob = t->operation(Opcode::MUL, t->constant(1), t->X());
        REQUIRE(ob == t->X());

        Cache::Id oc = t->operation(Opcode::MUL, t->X(), t->constant(0));
        REQUIRE(t->opcode(oc) == Opcode::CONST);
        REQUIRE(t->value(oc) == 0);

        Cache::Id od = t->operation(Opcode::MUL, t->constant(0), t->X());
        REQUIRE(t->opcode(od) == Opcode::CONST);
        REQUIRE(t->value(od) == 0);
    }
}

TEST_CASE("Collapsing constants")
{
    Cache::reset();
    auto t = Cache::instance();

    Cache::Id a = t->operation(Opcode::ADD, t->constant(4), t->constant(3));
    REQUIRE(t->opcode(a) == Opcode::CONST);
    REQUIRE(t->value(a) == 7);

    Cache::Id b = t->operation(Opcode::NEG, t->constant(4));
    REQUIRE(t->opcode(b) == Opcode::CONST);
    REQUIRE(t->value(b) == -4);
}

TEST_CASE("Collapsing affine trees")
{
    Cache::reset();
    auto t = Cache::instance();

    auto a_affine = t->affine(1.0, 0.0, 0.0, 3.0);
    /*
     * The tree should be of the form
     *              AFFINE
     *             /      \
     *          ADD         ADD
     *        /    \      /      \
     *      MUL    MUL   MUL     3
     *    /   \   /  \  /   \
     *   X    1  Y   0  Z   0
     *
     *  (and all of the zeros should point to the same token)
     */
    REQUIRE(a_affine == 12);

    auto collapsed = t->collapse(a_affine);
    REQUIRE(collapsed == 13);
}

TEST_CASE("Cache::findConnected")
{
    Cache::reset();
    auto t = Cache::instance();
    auto connected = t->findConnected(t->X());
    REQUIRE(connected.size() == 1);
}

TEST_CASE("Cache::var")
{
    Cache::reset();
    auto t = Cache::instance();
    auto a = t->var(3.1);
    auto b = t->var(3.2);
    REQUIRE(a != b);
}

TEST_CASE("Cache::checkCommutative")
{
    Cache::reset();
    auto t = Cache::instance();

    SECTION("No balancing")
    {
        auto a = t->operation(Opcode::MIN, t->X(), t->Y());
        REQUIRE(t->rank(a) == 1);
    }

    SECTION("LHS")
    {
        auto b = t->operation(Opcode::MIN, t->X(),
                 t->operation(Opcode::MIN, t->Y(),
                 t->operation(Opcode::MIN, t->Z(), t->constant(1.0f))));
        REQUIRE(t->rank(b) == 2);
    }

    SECTION("RHS")
    {
        auto b = t->operation(Opcode::MIN, t->operation(Opcode::MIN,
                 t->operation(Opcode::MIN, t->Z(), t->constant(1.0f)),
                     t->X()), t->Y());
        REQUIRE(t->rank(b) == 2);
    }
}
