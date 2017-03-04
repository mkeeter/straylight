#include "catch/catch.hpp"

#include "graph/async.hpp"

using namespace Graph;

TEST_CASE("AsyncRoot::run")
{
    AsyncRoot r;

    shared_queue<Command> in;
    auto& out = r.run(in);
    sleep(1);
    while(out.size())
    {
        out.pop();
    }

    SECTION("Inserting cell")
    {
        REQUIRE(out.size() == 0);
        in.push(Command::InsertCell(Tree::ROOT_SHEET, "a", "(+ 1 2)"));
        sleep(1);
        REQUIRE(out.size() == 3);
        {
            auto a = out.pop();
            REQUIRE(a.op == Response::CELL_INSERTED);
            REQUIRE(a.name == "a");
        }
        {
            auto a = out.pop();
            REQUIRE(a.op == Response::CELL_TYPE_CHANGED);
        }
        {
            auto a = out.pop();
            REQUIRE(a.op == Response::VALUE_CHANGED);
            REQUIRE(a.expr == "3");
        }
    }
    in.halt();
    r.wait();
}
