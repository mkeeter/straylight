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
    auto v = Tree::var();
    Evaluator e(v, {{v.id(), 3.14}});
    REQUIRE(e.eval(1.0, 2.0, 3.0) == Approx(3.14));
}

TEST_CASE("Evaluator::gradient")
{
    SECTION("constant + variable")
    {
        auto v = Tree::var();
        Evaluator e(v + 1.0, {{v.id(), 3.14}});
        REQUIRE(e.eval(1.0, 2.0, 3.0) == Approx(4.14));
        auto g = e.gradient(1, 2, 3);
        REQUIRE(g.size() == 1);
        REQUIRE(g.count(v.id()) == 1);
        REQUIRE(g.at(v.id()) == Approx(1));
    }

    SECTION("x * variable")
    {
        auto v = Tree::var();
        Evaluator e(Tree::X() * v, {{v.id(), 1}});
        {
            auto g = e.gradient(2, 0, 0);
            REQUIRE(g.size() == 1);
            REQUIRE(g.at(v.id()) == Approx(2));
        }
        {
            auto g = e.gradient(3, 0, 0);
            REQUIRE(g.at(v.id()) == Approx(3));
        }
    }

    SECTION("Multiple variables")
    {
        // Deliberately construct out of order
        auto a = Tree::var();
        auto c = Tree::var();
        auto b = Tree::var();

        Evaluator e(Tree(a*1 + b*2 + c*3),
                {{a.id(), 3}, {c.id(), 7}, {b.id(), 5}});
        REQUIRE(e.eval(0, 0, 0) == Approx(34));

        auto g = e.gradient(0, 0, 0);
        REQUIRE(g.at(a.id()) == Approx(1.0f));
        REQUIRE(g.at(b.id()) == Approx(2.0f));
        REQUIRE(g.at(c.id()) == Approx(3.0f));
    }
}

TEST_CASE("Evaluator copy-constructor")
{
    // Deliberately construct out of order
    auto a = Tree::var();
    auto c = Tree::var();
    auto b = Tree::var();

    Evaluator e_(Tree(a*1 + b*2 + c*3),
                {{a.id(), 3}, {c.id(), 7}, {b.id(), 5}});

    Evaluator e(e_);
    REQUIRE(e.eval(0, 0, 0) == Approx(34));
    auto g = e.gradient(0, 0, 0);
    REQUIRE(g.at(a.id()) == Approx(1.0f));
    REQUIRE(g.at(b.id()) == Approx(2.0f));
    REQUIRE(g.at(c.id()) == Approx(3.0f));
}

TEST_CASE("Evaluator::setVar")
{
    // Deliberately construct out of order
    auto a = Tree::var();
    auto c = Tree::var();
    auto b = Tree::var();

    Evaluator e(a*1 + b*2 + c*3,
                {{a.id(), 3}, {c.id(), 7}, {b.id(), 5}});
    REQUIRE(e.eval(0, 0, 0) == Approx(34));

    e.setVar(a.id(), 5);
    REQUIRE(e.eval(0, 0, 0) == Approx(36));
    e.setVar(b.id(), 0);
    REQUIRE(e.eval(0, 0, 0) == Approx(26));
    e.setVar(c.id(), 10);
    REQUIRE(e.eval(0, 0, 0) == Approx(35));
}

TEST_CASE("Evaluator::varValues")
{
    // Deliberately construct out of order
    auto a = Tree::var();
    Evaluator e(a, {{a.id(), 3}});

    {
        auto v = e.varValues();
        REQUIRE(v.size() == 1);
        REQUIRE(v.at(a.id()) == 3.0);
    }

    e.setVar(a.id(), 5.0);
    {
        auto v = e.varValues();
        REQUIRE(v.size() == 1);
        REQUIRE(v.at(a.id()) == 5.0);
    }
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

TEST_CASE("Matrix evaluation")
{
    auto t = Tree::X();

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
    auto t = Tree::X();

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

TEST_CASE("Evaluator::specialize")
{
    Evaluator e(min(Tree::X(), Tree::Y()));

    e.specialize(-1, 0, 0); // specialize to just "X"
    REQUIRE(e.eval(-2, 0, 0) == -2);
    REQUIRE(e.eval(4, 0, 0) == 4);
    REQUIRE(e.eval(4, 5, 0) == 4);
    REQUIRE(e.eval(10, 5, 0) == 10);

    e.pop();
    e.specialize(0, -1, 0); // specialize to just "X"
    REQUIRE(e.eval(-2, 0, 0) == 0);
    REQUIRE(e.eval(4, 0, 0) == 0);
    REQUIRE(e.eval(4, 5, 0) == 5);
    REQUIRE(e.eval(10, 5, 0) == 5);
}

TEST_CASE("Evaluator::accumulateFeatures")
{
    std::map<Evaluator::Feature, std::list<glm::vec3>> fs;
    SECTION("min")
    {
        Evaluator e(min(Tree::X(), Tree::Y()));

        fs.clear();
        e.accumulateFeatures(1, 0, 0, fs);
        e.accumulateFeatures(0, 1, 0, fs);
        REQUIRE(fs.size() == 2);

        fs.clear();
        e.accumulateFeatures(1, 0, 0, fs);
        e.accumulateFeatures(2, 1, 0, fs);
        REQUIRE(fs.size() == 1);

        fs.clear();
        e.accumulateFeatures(1, 2, 0, fs);
        e.accumulateFeatures(0, 1, 0, fs);
        REQUIRE(fs.size() == 1);

        fs.clear();
        e.accumulateFeatures(0, 0, 0, fs);
        REQUIRE(fs.size() == 2);
    }
    SECTION("Push / pop behavior")
    {
        Evaluator e(max(Tree::X(), Tree::Y()));

        e.set({0,0}, {0,0}, {0,1});
        e.interval();
        e.push();
        fs.clear();
        e.accumulateFeatures(0, 0, 0, fs);
        REQUIRE(fs.size() == 2);
        e.pop();

        e.set({0,1}, {1,2}, {0,1});
        e.interval();
        e.push();
        fs.clear();
        e.accumulateFeatures(0, 0, 0, fs);
        REQUIRE(fs.size() == 2);
        e.pop();
    }
    SECTION("max")
    {
        Evaluator e(max(Tree::X(), Tree::Y()));

        fs.clear();
        e.accumulateFeatures(1, 0, 0, fs);
        e.accumulateFeatures(0, 1, 0, fs);
        REQUIRE(fs.size() == 2);

        fs.clear();
        e.accumulateFeatures(1, 0, 0, fs);
        e.accumulateFeatures(2, 1, 0, fs);
        REQUIRE(fs.size() == 1);

        fs.clear();
        e.accumulateFeatures(1, 2, 0, fs);
        e.accumulateFeatures(0, 1, 0, fs);
        REQUIRE(fs.size() == 1);

        fs.clear();
        e.accumulateFeatures(0, 0, 0, fs);
        e.accumulateFeatures(0, 0, 0, fs);
        e.values(2);
        REQUIRE(fs.size() == 2);
    }
    SECTION("abs")
    {
        Evaluator e(abs(Tree::X()));

        fs.clear();
        e.accumulateFeatures(1, 0, 0, fs);
        e.accumulateFeatures(-1, 0, 0, fs);
        REQUIRE(fs.size() == 2);

        fs.clear();
        e.accumulateFeatures(1, 0, 0, fs);
        e.accumulateFeatures(2, 0, 0, fs);
        REQUIRE(fs.size() == 1);

        fs.clear();
        e.accumulateFeatures(-1, 0, 0, fs);
        e.accumulateFeatures(-2, 0, 0, fs);
        REQUIRE(fs.size() == 1);

        fs.clear();
        e.accumulateFeatures(0, 0, 0, fs);
        e.accumulateFeatures(0, 0, 0, fs);
        REQUIRE(fs.size() == 2);
    }
    SECTION("mod")
    {
        Evaluator e(mod(Tree::X(), Tree(2)));

        fs.clear();
        e.accumulateFeatures(1, 0, 0, fs);
        REQUIRE(fs.size() == 1);
        e.accumulateFeatures(3, 0, 0, fs);
        REQUIRE(fs.size() == 2);
        e.accumulateFeatures(3.5, 0, 0, fs);
        REQUIRE(fs.size() == 2);
        e.accumulateFeatures(5, 0, 0, fs);
        REQUIRE(fs.size() == 3);
        e.accumulateFeatures(7, 0, 0, fs);
        REQUIRE(fs.size() == 4);
    }
}
