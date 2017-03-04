#include <catch/catch.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include "kernel/tree/tree.hpp"
#include "kernel/eval/evaluator.hpp"

using namespace Kernel;

TEST_CASE("Principle variable evaluation")
{
    SECTION("X")
    {
        Evaluator e(Tree::X());
        REQUIRE(e.eval(1.0, 2.0, 3.0) == 1.0);
    }

    SECTION("Y")
    {
        Evaluator e(Tree::Y());
        REQUIRE(e.eval(1.0, 2.0, 3.0) == 2.0);
    }
}

TEST_CASE("Constant evaluation")
{
    Evaluator e(Tree(3.14));
    REQUIRE(e.eval(1.0, 2.0, 3.0) == Approx(3.14));
}

TEST_CASE("Secondary variable evaluation")
{
    Evaluator e(Tree::var(3.14));
    REQUIRE(e.eval(1.0, 2.0, 3.0) == Approx(3.14));
}

TEST_CASE("Evaluator::gradient")
{
    SECTION("constant + variable")
    {
        auto v = Tree::var(3.14);
        Evaluator e(v + 1.0);
        REQUIRE(e.eval(1.0, 2.0, 3.0) == Approx(4.14));
        auto g = e.gradient(1, 2, 3);
        REQUIRE(g.size() == 1);
        REQUIRE(g.count(v.var()) == 1);
        REQUIRE(g.at(v.var()) == Approx(1));
    }

    SECTION("x * variable")
    {
        auto v = Tree::var(1.0);
        Evaluator e(Tree::X() * v);
        {
            auto g = e.gradient(2, 0, 0);
            REQUIRE(g.size() == 1);
            REQUIRE(g.at(v.var()) == Approx(2));
        }
        {
            auto g = e.gradient(3, 0, 0);
            REQUIRE(g.at(v.var()) == Approx(3));
        }
    }

    SECTION("Multiple variables")
    {
        // Deliberately construct out of order
        auto a = Tree::var(3.0);
        auto c = Tree::var(7.0);
        auto b = Tree::var(5.0);

        Evaluator e(Tree(a*1 + b*2 + c*3));
        REQUIRE(e.eval(0, 0, 0) == Approx(34));
        auto g = e.gradient(0, 0, 0);
        REQUIRE(g.at(a.var()) == Approx(1.0f));
        REQUIRE(g.at(b.var()) == Approx(2.0f));
        REQUIRE(g.at(c.var()) == Approx(3.0f));
    }
}

TEST_CASE("Evaluator copy-constructor")
{
    // Deliberately construct out of order
    auto a = Tree::var(3.0);
    auto c = Tree::var(7.0);
    auto b = Tree::var(5.0);

    Evaluator e_(Tree(a*1 + b*2 + c*3));

    Evaluator e(e_);
    REQUIRE(e.eval(0, 0, 0) == Approx(34));
    auto g = e.gradient(0, 0, 0);
    REQUIRE(g.at(a.var()) == Approx(1.0f));
    REQUIRE(g.at(b.var()) == Approx(2.0f));
    REQUIRE(g.at(c.var()) == Approx(3.0f));
}

TEST_CASE("Evaluator::setVar")
{
    // Deliberately construct out of order
    auto a = Tree::var(3.0);
    auto c = Tree::var(7.0);
    auto b = Tree::var(5.0);

    Evaluator e(a*1 + b*2 + c*3);
    REQUIRE(e.eval(0, 0, 0) == Approx(34));

    e.setVar(a.var(), 5);
    REQUIRE(e.eval(0, 0, 0) == Approx(36));
    e.setVar(b.var(), 0);
    REQUIRE(e.eval(0, 0, 0) == Approx(26));
    e.setVar(c.var(), 10);
    REQUIRE(e.eval(0, 0, 0) == Approx(35));
}

TEST_CASE("Evaluator::varValues")
{
    // Deliberately construct out of order
    auto a = Tree::var(3.0);
    Evaluator e(a);

    {
        auto v = e.varValues();
        REQUIRE(v.size() == 1);
        REQUIRE(v.at(a.var()) == 3.0);
    }

    e.setVar(a.var(), 5.0);
    {
        auto v = e.varValues();
        REQUIRE(v.size() == 1);
        REQUIRE(v.at(a.var()) == 5.0);
    }
}

TEST_CASE("Evaluator::updateVars")
{
    // Deliberately construct out of order
    auto a = Tree::var(3.0);
    Evaluator e(a);

    a.setValue(5.0);
    REQUIRE(e.updateVars(a.cache()));
    REQUIRE(!e.updateVars(a.cache()));

    auto v = e.varValues();
    REQUIRE(v.size() == 1);
    REQUIRE(v.at(a.var()) == 5.0);
}

TEST_CASE("Float evaluation")
{
    SECTION("X + 1")
    {
        Evaluator e(Tree::X() + 1);
        REQUIRE(e.eval(1.0, 2.0, 3.0) == 2.0);
    }

    SECTION("X + Z")
    {
        Evaluator e(Tree::X() + Tree::Z());
        REQUIRE(e.eval(1.0, 2.0, 3.0) == 4.0);
    }
}

TEST_CASE("Interval evaluation")
{
    Evaluator e(Tree::X() + 1);

    Interval arg(1, 2);
    auto out = e.eval(arg, arg, arg);

    REQUIRE(out.lower() == 2.0);
    REQUIRE(out.upper() == 3.0);
}

TEST_CASE("Push / pop behavior")
{
    Evaluator e(min(Tree::X() + 1, Tree::Y() + 1));

    // Store -3 in the rhs's value
    REQUIRE(e.eval(1.0f, -3.0f, 0.0f) == -2);

    // Do an interval evaluation that will lead to disabling the rhs
    auto i = e.eval(Interval(-5, -4), Interval(8, 9), Interval(0, 0));
    REQUIRE(i.lower() == -4);
    REQUIRE(i.upper() == -3);

    // Push (which should disable the rhs of min
    e.push();

    // Check to make sure that the push disabled something
    CAPTURE(e.utilization());
    REQUIRE(e.utilization() < 1);

    // Require that the evaluation gets 1
    REQUIRE(e.eval(1.0f, 2.0f, 0.0f) == 2);
}

TEST_CASE("Affine evaluation")
{
    Evaluator e(Tree::affine(1, 0, 0, 0));
    REQUIRE(e.eval(1.0, 2.0, 3.0) == 1.0);
}

TEST_CASE("Matrix evaluation")
{
    Tree t(Tree::affine(1, 0, 0, 0));

    SECTION("Default matrix")
    {
        Evaluator e(t);
        REQUIRE(e.eval(1.0, 2.0, 3.0) == 1.0);
    }

    SECTION("Scaling")
    {
        Evaluator e(t, glm::scale(glm::mat4(), {0.5, 1.0, 1.0}));
        REQUIRE(e.eval(1.0, 2.0, 3.0) == 0.5);
    }

    SECTION("Swapping")
    {
        Evaluator e(t, glm::rotate(glm::mat4(), -(float)M_PI * 0.5f,
                               {0.0, 0.0, 1.0}));
        REQUIRE(e.eval(1.0, 2.0, 3.0) == Approx(2.0));
    }

    SECTION("Offset")
    {
        Evaluator e(t, glm::translate(glm::mat4(), {0.5, 0.0, 0.0}));
        REQUIRE(e.eval(1.0, 2.0, 3.0) == 1.5);
    }
}

TEST_CASE("Matrix normals")
{
    Tree t(Tree::affine(1, 0, 0, 0));

    SECTION("Swapping")
    {
        Evaluator e(t, glm::rotate(glm::mat4(), -(float)M_PI * 0.5f,
                               {0.0, 0.0, 1.0}));
        e.set(1, 2, 3, 0);
        auto out = e.derivs(1);

        REQUIRE(out.dx[0] == Approx(0));
        REQUIRE(out.dy[0] == Approx(1));
        REQUIRE(out.dz[0] == Approx(0));
    }

    SECTION("Swapping")
    {
        Evaluator e(t, glm::rotate(glm::mat4(), -(float)M_PI * 0.5f,
                               {0.0, 0.0, 1.0}));
        e.set(1, 2, 3, 0);
        auto out = e.derivs(1);

        REQUIRE(out.dx[0] == Approx(0));
        REQUIRE(out.dy[0] == Approx(1));
        REQUIRE(out.dz[0] == Approx(0));
    }
}

TEST_CASE("Evaluator::derivs")
{
    SECTION("X")
    {
        Evaluator e(Tree::X());
        e.set(0, 0, 0, 0);
        e.set(1, 2, 3, 1);
        auto d = e.derivs(2);

        // Values = x
        REQUIRE(d.v[0] == 0.0);
        REQUIRE(d.v[1] == 1.0);

        // d/dx = 1
        REQUIRE(d.dx[0] == 1.0);
        REQUIRE(d.dx[1] == 1.0);

        // d/dy = 0
        REQUIRE(d.dy[0] == 0.0);
        REQUIRE(d.dy[1] == 0.0);

        // d/dz = 0
        REQUIRE(d.dz[0] == 0.0);
        REQUIRE(d.dz[1] == 0.0);
    }

    SECTION("X + Z")
    {
        Evaluator e(Tree::X() + Tree::Z());

        e.set(1, 1, 1, 0);
        e.set(1, 2, 3, 1);
        auto d = e.derivs(2);

        // Values = x
        REQUIRE(d.v[0] == 2.0);
        REQUIRE(d.v[1] == 4.0);

        // d/dx = 1
        REQUIRE(d.dx[0] == 1.0);
        REQUIRE(d.dx[1] == 1.0);

        // d/dy = 0
        REQUIRE(d.dy[0] == 0.0);
        REQUIRE(d.dy[1] == 0.0);

        // d/dz = 1
        REQUIRE(d.dz[0] == 1.0);
        REQUIRE(d.dz[1] == 1.0);
    }
}

TEST_CASE("Evaluator::toTree")
{
    boost::bimap<Cache::VarId, Cache::VarId> vm;

    SECTION("Tree without XYZ")
    {
        auto a = Tree::var(3.0);
        auto b = a + Tree(4.5);

        auto e = Evaluator(b);

        auto t = e.toTree(vm);

        REQUIRE(vm.size() == 1);
        REQUIRE(vm.left.count(a.var()) == 1);
        auto a_ = vm.left.at(a.var());

        REQUIRE(t.opcode() == Opcode::ADD);

        REQUIRE(t.lhs().opcode() == Opcode::VAR);
        REQUIRE(t.lhs().value() == 3.0);
        REQUIRE(t.lhs().var() == a_);

        REQUIRE(t.rhs().opcode() == Opcode::CONST);
        REQUIRE(t.rhs().value() == 4.5);

        // Re-run and confirm that we get the same tree
        auto t_ = e.toTree(vm);
        REQUIRE(vm.size() == 1);
        REQUIRE(t == t_);
    }

    SECTION("Tree with XYZ")
    {
        auto a = Tree::var(3.0);
        auto b = a + Tree::X();
        REQUIRE(b.opcode() == Opcode::ADD);

        auto e = Evaluator(b);

        auto t = e.toTree(vm);

        REQUIRE(vm.size() == 1);
        REQUIRE(vm.left.count(a.var()) == 1);
        auto a_ = vm.left.at(a.var());

        REQUIRE(t.opcode() == Opcode::ADD);

        REQUIRE(t.lhs().opcode() == Opcode::VAR);
        REQUIRE(t.lhs().value() == 3.0);
        REQUIRE(t.lhs().var() == a_);

        REQUIRE(t.rhs().opcode() == Opcode::VAR_X);
    }

    SECTION("Constant")
    {
        auto a = Tree(3.0);
        auto e = Evaluator(a);

        auto a_ = e.toTree(vm);
        REQUIRE(a_.opcode() == Opcode::CONST);
        REQUIRE(a_.value() == 3.0);
    }

    SECTION("X")
    {
        auto a = Tree::X();
        auto e = Evaluator(a);

        auto a_ = e.toTree(vm);
        REQUIRE(a_.opcode() == Opcode::VAR_X);
    }

    SECTION("Y")
    {
        auto a = Tree::Y();
        auto e = Evaluator(a);

        auto a_ = e.toTree(vm);
        REQUIRE(a_.opcode() == Opcode::VAR_Y);
    }

    SECTION("Var")
    {
        auto a = Tree::var(5.5);
        auto e = Evaluator(a);

        auto a_ = e.toTree(vm);
        REQUIRE(a_.opcode() == Opcode::VAR);
        REQUIRE(a.value() == 5.5);

        REQUIRE(vm.size() == 1);
        auto v = vm.left.at(a.var());
        REQUIRE(a_.var() == v);
    }
}
