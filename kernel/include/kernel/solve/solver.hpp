#include <map>

#include "glm/vec3.hpp"

#include "kernel/tree/cache.hpp"
#include "kernel/eval/evaluator.hpp"

namespace Kernel {

class Tree;

namespace Solver
{
    typedef std::map<Cache::VarId, float> Solution;

    /*
     *  Finds a set of variables that drive t to zero
     *
     *  v is the x,y,z coordinates at which to solve
     *  Initial conditions are the variable values
     */
    std::pair<float, Solution> findRoot(const Tree& t, const glm::vec3 v={0,0,0});
    std::pair<float, Solution> findRoot(Evaluator& t, const glm::vec3 v={0,0,0});

}   // namespace Solver
}   // namespace Kernel
