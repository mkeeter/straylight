#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "root.hpp"
#include "cell.hpp"

TEST_CASE("Wat")
{
    std::unique_ptr<Graph::Root> root(new Graph::Root());
    auto c = root->insertCell(root->sheet.get(), "x", "(+ 1 2)");
    REQUIRE(c->values.size() == 1);
    REQUIRE(c->values.count({root->instance.get()}) == 1);
    REQUIRE(c->strs[{root->instance.get()}] == "3");
}

int main(int argc, char** argv)
{
    return Catch::Session().run(argc, argv);
}
