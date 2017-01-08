#include <map>

#include "glm/vec3.hpp"

#include "kernel/tree/cache.hpp"

namespace Kernel {

class Tree;

namespace Solver
{
    typedef std::map<Cache::Id, float> Solution;
    std::pair<float, Solution> findRoot(const Tree& t, const glm::vec3 v={0,0,0});
}   // namespace Solver

}   // namespace Kernel
