#include <algorithm>

#include "graph/key.hpp"

namespace Graph
{

CellKey::CellKey(CellId i, Cell::Env e) : id(i), env(e)
{
    /* Nothing to do here */
}

bool CellKey::operator==(const CellKey& other) const
{
    return id == other.id && env == other.env;
}

bool CellKey::operator!=(const CellKey& other) const
{
    return !(other == *this);
}

bool CellKey::operator<(const CellKey& other) const
{
    return (id != other.id) ? (id < other.id) : (env < other.env);
}

bool CellKey::specializes(const CellKey& other) const
{
    return id == other.id &&
           std::mismatch(other.env.begin(), other.env.end(),
                         env.begin()).first == other.env.end();

}

}   // namespace Graph
