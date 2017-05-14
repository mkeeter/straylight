#pragma once

#include <list>
#include <glm/vec3.hpp>

#include "kernel/eval/clause.hpp"

namespace Kernel
{

class Feature
{
public:
    struct Choice
    {
        const Clause::Id id;
        const int choice;
    };

    /*
     *  Checks to see whether a particular epsilon is compatible with
     *  all of the other epsilons in the system.
     *  This is a slow (worst-case O(n^3)) operation, but it should be called
     *  rarely and so doesn't need to be optimized yet.
     */
    bool isCompatible(glm::vec3 e) const;

    /*
     *  If incompatible, does nothing and returns false
     *  Otherwise, pushes to the front of the choice list and returns true
     */
    bool push(glm::vec3 e, Choice c={0, 0});

    /*
     *  Accessor method for the choice list
     */
    const std::list<Choice>& getChoices() const { return choices; }

    /*
     *  Top-level derivative (set manually)
     */
    glm::vec3 deriv;

protected:
    std::list<Choice> choices;
    std::list<glm::vec3> epsilons;
};

}   // namespace Kernel
