#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "root.hpp"

TEST_CASE("Wat")
{
    std::unique_ptr<Graph::Root> root(new Graph::Root());
}

int main(int argc, char** argv)
{
    return Catch::Session().run(argc, argv);
}
