#pragma once

#include <list>
#include <glm/vec3.hpp>

namespace Kernel
{

class Feature
{
public:
    /*
     *  Checks to see whether a particular epsilon is compatible with
     *  all of the other epsilons in the system.
     *  This is a slow (worst-case O(n^3)) operation, but it should be called
     *  rarely and so doesn't need to be optimized yet.
     */
    bool isCompatible(glm::vec3 e) const;

    bool push(glm::vec3 e, int choice=0);
    glm::vec3 deriv() const { return d; }
    void setDeriv(glm::vec3 d_) { d = d_; }
    const std::list<int>& getChoices() const { return choices; }

protected:
    std::list<int> choices;
    std::list<glm::vec3> epsilons;
    glm::vec3 d;
};

}   // namespace Kernel
