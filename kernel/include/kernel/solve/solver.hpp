#pragma once
#include <map>

#include "glm/vec3.hpp"

#include "kernel/tree/cache.hpp"
#include "kernel/eval/evaluator.hpp"

namespace Kernel {

class Tree;

namespace Solver
{
    typedef std::map<Cache::VarId, float> Solution;
    typedef std::set<Cache::VarId> Mask;

    /*
     *  Finds a set of variables that drive t to zero
     *
     *  v is the x,y,z coordinates at which to solve
     *  Initial conditions are the variable values
     */
    std::pair<float, Solution> findRoot(
            const Tree& t, const glm::vec3 v={0,0,0},
            const Mask& mask=Mask(), unsigned gas=25000);
    std::pair<float, Solution> findRoot(
            Evaluator& t, const glm::vec3 v={0,0,0},
            const Mask& mask=Mask(), unsigned gas=25000);

}   // namespace Solver
}   // namespace Kernel
