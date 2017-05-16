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

    // Return early if the epsilon is already in the list
    for (const auto& i : epsilons)
    {
        if (e == i)
        {
            return true;
        }
    }

    // Otherwise, we construct every possible plane and check against
    // every remaining point to make sure they work
    auto es = epsilons;
    es.push_back(e);

    // Yes, this is an O(n^3) loop
    // It's far from optimal, but will suffice unless people start making
    // deliberately pathological models.
    for (auto a=es.begin(); a != es.end(); ++a)
    {
        for (auto b=es.begin(); b != es.end(); ++b)
        {
            if (a == b || glm::dot(*a, *b) == -1)
            {
                continue;
            }
            const auto norm = glm::cross(*a, *b);
            int sign = 0;
            bool passed = true;
            for (auto c=es.begin(); passed && c != es.end(); ++c)
            {
                if (a == c || b == c)
                {
                    continue;
                }
                auto d = glm::dot(norm, *c);
                if (d < 0)
                {
                    passed &= (sign <= 0);
                    sign = -1;
                }
                else if (d > 0)
                {
                    passed &= (sign >= 0);
                    sign = 1;
                }
                else
                {
                    passed = false;
                }
            }
            if (passed)
            {
                return true;
            }
        }
    }
    return false;
}

bool Feature::push(glm::vec3 e, Choice choice)
{
    if (isCompatible(e))
    {
        choices.push_front(choice);

        // Store the epsilon if it isn't already present
        e /= glm::length(e);
        for (auto i : epsilons)
        {
            if (e == i)
            {
                return true;
            }
        }
        epsilons.push_back(e);
        return true;
    }
    else
    {
        return false;
    }
}

}   // namespace Kernel
