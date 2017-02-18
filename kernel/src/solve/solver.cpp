#include <numeric>

#include "kernel/solve/solver.hpp"
#include "kernel/tree/tree.hpp"
#include "kernel/eval/evaluator.hpp"

namespace Kernel {

namespace Solver
{

std::pair<float, Solution> findRoot(const Tree& t, const glm::vec3 v,
                                    const Mask& mask)
{
    Evaluator e(t);
    return findRoot(e, v, mask);
}

std::pair<float, Solution> findRoot(Evaluator& e, const glm::vec3 v,
                                    const Mask& mask)
{
    const float EPSILON = 1e-6;

    auto filter = [&](std::map<Cache::VarId, float>& vs){
        for (const auto& v : mask)
        {
            vs.erase(v);
        }
    };
    // Find our initial variables and residual
    auto vars = e.varValues();
    filter(vars);

    float r = e.eval(v.x, v.y, v.z);

    bool converged = false;

    while (!converged && fabs(r) >= EPSILON)
    {
        // Vars should be set from the most recent evaluation
        printf("Residual: %.2f\n", r);
        auto ds = e.gradient(v.x, v.y, v.z);
        filter(ds);
        for (auto d : ds)
        {
            printf("%i: v: %.2f\t d: %.2f\n", d.first.i, vars[d.first], d.second);
        }

        // Break if all of our gradients are nearly zero
        if (std::all_of(ds.begin(), ds.end(),
            [&EPSILON](decltype(ds)::value_type itr) {
                return fabs(itr.second) < EPSILON;
            }))
        {
            break;
        }

        // Solve for step size using a backtracking line search
        const float slope = std::accumulate(ds.begin(), ds.end(), 0.0f,
                [](float v, const decltype(ds)::value_type& itr) {
                    return v + pow(itr.second, 2); });
        printf("Slope: %.2f\n", slope);

        for (float step = r / slope; true; step /= 2)
        {
            for (const auto& v : vars)
            {
                printf("\t\t%i: v: %.2f\n", v.first.i, v.second - step * ds.at(v.first));
                e.setVar(v.first, v.second - step * ds.at(v.first));
            }

            // Get new residual
            const auto r_ = e.eval(v.x, v.y, v.z);
            printf("\t%.2f\t(%.2f)\n", r_, r);

            // Find change in residuals
            const auto diff = r - r_;

            // If we've satisfied the Armijo–Goldstein condition, then break
            if (slope < EPSILON || r_ < EPSILON || diff / step >= slope * 0.5)
            {
                // If residuals are converging, then exit outer loop too
                converged = fabs(diff) < EPSILON;
                r = r_;
                printf("Got step %.2f\n", step);
                break;
            }
        }

        // Extract new variable table
        vars = e.varValues();
        filter(vars);
        printf("\n");
    }
    return {r, vars};
}

} // namespace Solver

}   // namespace Kernel
