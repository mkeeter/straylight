#include <glm/glm.hpp>
#include "kernel/eval/feature.hpp"

namespace Kernel {

bool Feature::isCompatible(glm::vec3 e) const
{
    {   // Normalize based on vector length
        const auto norm = glm::length(e);
        if (norm == 0)
        {
            return false;
        }
        e /= norm;
    }

    if (epsilons.size() == 0)
    {
        return true;
    }
    else if (epsilons.size() == 1)
    {
        return glm::dot(e, epsilons.front()) != -1;
    }
    return true;
}

bool Feature::push(glm::vec3 e)
{
    if (isCompatible(e))
    {
        epsilons.push_back(e / glm::length(e));
        return true;
    }
    else
    {
        return false;
    }
}

}   // namespace Kernel
