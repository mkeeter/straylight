#pragma once

namespace Graph
{
class Graph;

/*
 *  The Caller class is used to abstract away the "call a sheet" operation,
 *  so it can be passed to interpreter and symbol table classes without
 *  providing access to the entire Graph's interface.
 */
class Caller
{
public:
    Caller(Graph& g) : g(g) { /* Nothing to do here */ }
protected:
    Graph& g;
};

}   // namespace Graph
