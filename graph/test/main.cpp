#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "root.hpp"
#include "cell.hpp"

TEST_CASE("Simple evaluation")
{
    std::unique_ptr<Graph::Root> root(new Graph::Root());
    auto c = root->insertCell(root->sheet.get(), "x", "(+ 1 2)");
    REQUIRE(c->values.size() == 1);
    REQUIRE(c->values.count({root->instance.get()}) == 1);
    REQUIRE(c->strs[{root->instance.get()}] == "3");
}

TEST_CASE("Evaluation with lookups")
{
    std::unique_ptr<Graph::Root> root(new Graph::Root());
    auto x = root->insertCell(root->sheet.get(), "x", "1");
    auto y = root->insertCell(root->sheet.get(), "y", "(+ 1 (x))");
    REQUIRE(y->strs[{root->instance.get()}] == "2");
}

TEST_CASE("Re-evaluation on parent change")
{
    std::unique_ptr<Graph::Root> root(new Graph::Root());
    auto x = root->insertCell(root->sheet.get(), "x", "1");
    auto y = root->insertCell(root->sheet.get(), "y", "(+ 1 (x))");
    root->editCell(x, "2");
    REQUIRE(y->strs[{root->instance.get()}] == "3");
}

TEST_CASE("Multiple evaluation paths")
{
    std::unique_ptr<Graph::Root> root(new Graph::Root());
    auto x = root->insertCell(root->sheet.get(), "x", "1");
    auto a = root->insertCell(root->sheet.get(), "a", "(+ 2 (x))");
    auto y = root->insertCell(root->sheet.get(), "y", "(+ (a) (x))");
    root->editCell(x, "3");
    REQUIRE(y->strs[{root->instance.get()}] == "8");
}

TEST_CASE("Re-evaluation on cell insertion")
{
    std::unique_ptr<Graph::Root> root(new Graph::Root());
    auto y = root->insertCell(root->sheet.get(), "y", "(+ 1 (x))");
    auto x = root->insertCell(root->sheet.get(), "x", "1");
    REQUIRE(y->strs[{root->instance.get()}] == "2");
}

int main(int argc, char** argv)
{
    return Catch::Session().run(argc, argv);
}
