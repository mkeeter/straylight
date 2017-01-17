#include <catch/catch.hpp>

#include "kernel/tree/tree.hpp"
#include "util/glm.hpp"

using namespace Kernel;

TEST_CASE("Joining two trees")
{
    auto t = Tree::X() + 1;

    REQUIRE(t.opcode() == Opcode::ADD);
    REQUIRE(t.lhs().opcode() == Opcode::VAR_X);
    REQUIRE(t.rhs().opcode() == Opcode::CONST);
    REQUIRE(t.rhs().value() == 1);
}

TEST_CASE("Affine math")
{
    SECTION("Affine plus constant")
    {
        Tree t = Tree::affine(1, 0, 0, 0) + 1;

        REQUIRE(t.opcode() == Opcode::AFFINE_VEC);
        REQUIRE(t.getAffine() == glm::vec4(1, 0, 0, 1));
    }

    SECTION("Constant plus affine")
    {
        Tree t = 2 + Tree::affine(1, 0, 0, 0);

        REQUIRE(t.opcode() == Opcode::AFFINE_VEC);
        REQUIRE(t.getAffine() == glm::vec4(1, 0, 0, 2));
    }

    SECTION("Affine plus affine")
    {
        Tree t = Tree::affine(1, 2, 3, 4) + Tree::affine(2, 4, 6, 8);
        REQUIRE(t.opcode() == Opcode::AFFINE_VEC);
        REQUIRE(t.getAffine() == glm::vec4(3, 6, 9, 12));
    }

    SECTION("Affine minus constant")
    {
        Tree t = Tree::affine(1, 0, 0, 0) - 1;

        REQUIRE(t.opcode() == Opcode::AFFINE_VEC);
        REQUIRE(t.getAffine() == glm::vec4(1, 0, 0, -1));
    }

    SECTION("Constant minus affine")
    {
        Tree t = 2 - Tree::affine(1, 0, 0, 0);

        REQUIRE(t.opcode() == Opcode::AFFINE_VEC);
        REQUIRE(t.getAffine() == glm::vec4(-1, 0, 0, 2));
    }

    SECTION("Affine minus affine")
    {
        Tree t = Tree::affine(1, 2, 3, 4) - Tree::affine(2, 4, 6, 8);
        REQUIRE(t.opcode() == Opcode::AFFINE_VEC);
        REQUIRE(t.getAffine() == glm::vec4(-1, -2, -3, -4));
    }

    SECTION("Affine times constant")
    {
        Tree t = Tree::affine(1, 2, 3, 4) * 2;
        REQUIRE(t.opcode() == Opcode::AFFINE_VEC);
        REQUIRE(t.getAffine() == glm::vec4(2, 4, 6, 8));
    }

    SECTION("Constant times affine")
    {
        Tree t = 2 * Tree::affine(1, 2, 3, 4);
        REQUIRE(t.opcode() == Opcode::AFFINE_VEC);
        REQUIRE(t.getAffine() == glm::vec4(2, 4, 6, 8));
    }

    SECTION("Affine divided by constant")
    {
        Tree t = Tree::affine(1, 2, 3, 4) / 2;
        REQUIRE(t.opcode() == Opcode::AFFINE_VEC);
        REQUIRE(t.getAffine() == glm::vec4(0.5, 1, 1.5, 2));
    }
}

TEST_CASE("Tree::collapse")
{
    SECTION("Base case")
    {
        auto t = Tree::affine(1, 0, 0, 0);
        auto t_ = t.collapse();
        REQUIRE(t_.opcode() == Opcode::VAR_X);
        REQUIRE(t_.rank() == 0);
    }

    SECTION("Sum")
    {
        auto t = Tree::affine(1, 1, 0, 0);
        auto t_ = t.collapse();
        REQUIRE(t_.opcode() == Opcode::ADD);
        REQUIRE(t_.lhs().opcode() == Opcode::VAR_X);
        REQUIRE(t_.rhs().opcode() == Opcode::VAR_Y);
        REQUIRE(t_.rank() == 1);
    }

    SECTION("Multiplication")
    {
        auto t = Tree::affine(2, 3, 0, 0);
        auto t_ = t.collapse();
        REQUIRE(t_.opcode() == Opcode::ADD);
        REQUIRE(t_.lhs().opcode() == Opcode::MUL);
        REQUIRE(t_.rhs().opcode() == Opcode::MUL);

        REQUIRE(t_.lhs().lhs().opcode() == Opcode::VAR_X);
        REQUIRE(t_.lhs().rhs().opcode() == Opcode::CONST);
        REQUIRE(t_.lhs().rhs().value() == 2);
        REQUIRE(t_.rhs().lhs().opcode() == Opcode::VAR_Y);
        REQUIRE(t_.rhs().rhs().opcode() == Opcode::CONST);
        REQUIRE(t_.rhs().rhs().value() == 3);
    }

    SECTION("Constant")
    {
        auto t = Tree::affine(1, 0, 0, 3);
        auto t_ = t.collapse();
        REQUIRE(t_.opcode() == Opcode::ADD);
        REQUIRE(t_.lhs().opcode() == Opcode::VAR_X);
        REQUIRE(t_.rhs().opcode() == Opcode::CONST);
        REQUIRE(t_.rhs().value() == 3);
    }

    SECTION("Not affine at all")
    {
        auto t = Tree::X();
        auto t_ = t.collapse();
        REQUIRE(t_.opcode() == Opcode::VAR_X);
    }
}

TEST_CASE("Tree::var")
{
    Cache::reset();

    auto a = Tree::var(3.0);
    auto b = Tree::var(5.0);
    auto c = Tree::var(7.0);

    REQUIRE(a.var() == 1);
    REQUIRE(b.var() == 2);
    REQUIRE(c.var() == 3);
}
