#pragma once

#include <array>
#include <unordered_map>
#include <vector>

#include <glm/mat4x4.hpp>

#include "kernel/eval/result.hpp"
#include "kernel/eval/interval.hpp"
#include "kernel/eval/clause.hpp"
#include "kernel/tree/tree.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace Kernel {

class EvaluatorBase
{
public:
    /*
     *  Construct an evaluator for the given tree
     */
    EvaluatorBase(const Tree root, const glm::mat4& M=glm::mat4());

    /*
     *  Copy constructor
     */
    EvaluatorBase(const EvaluatorBase& other);

    /*
     *  Converts this evaluator back into a tree
     *
     *  The VarId map defines how to convert variables from the original
     *  cache to the new cache (as they are expected to live in different
     *  threads).
     */
    Tree toTree(boost::bimap<Cache::VarId, Cache::VarId>& vars) const;

    /*
     *  Single-argument evaluation
     */
    float eval(float x, float y, float z);
    Interval eval(Interval x, Interval y, Interval z);

    /*
     *  Evaluates a set of floating-point results
     *  (which have been loaded with set)
     */
    const float* values(Result::Index count);

    /*
     *  Helper struct when returning derivatives
     */
    struct Derivs {
        const float* v;
        const float* dx;
        const float* dy;
        const float* dz;
    };

    /*
     *  Evaluate a set of gradients, returning a tuple
     *      value, dx, dy, dz
     *
     *  Values must have been previously loaded by set
     */
    Derivs derivs(Result::Index count);

    /*
     *  Returns the gradient with respect to all VARs
     */
    std::map<Cache::VarId, float> gradient(float x, float y, float z);

    /*
     *  Evaluates a single interval (stored with set)
     */
    Interval interval();

    /*
     *  Stores the given value in the result arrays
     *  (inlined for efficiency)
     */
    void set(float x, float y, float z, Result::Index index)
    {
        result.f[X][index] = M[0][0] * x + M[1][0] * y + M[2][0] * z + M[3][0];
        result.f[Y][index] = M[0][1] * x + M[1][1] * y + M[2][1] * z + M[3][1];
        result.f[Z][index] = M[0][2] * x + M[1][2] * y + M[2][2] * z + M[3][2];
    }

    /*
     *  Unsafe setter (which requires a call to applyTransform afterwards)
     */
    void setRaw(float x, float y, float z, Result::Index index)
    {
        result.f[X][index] = x;
        result.f[Y][index] = y;
        result.f[Z][index] = z;
    }

    /*
     *  Applies M to values stored by set
     */
    void applyTransform(Result::Index count);

    /*
     *  Stores the given interval in the result objects
     */
    void set(Interval X, Interval Y, Interval Z);

    /*
     *  Pushes into a subinterval, disabling inactive nodes
     */
    void push();

    /*
     *  Pops out of interval evaluation, re-enabling disabled nodes
     */
    void pop();

    /*
     *  Returns the fraction active / total nodes
     *  (to check how well disabling is working)
     */
    double utilization() const;

    /*
     *  Sets the global matrix transform
     *  Invalidates all positions and results
     */
    void setMatrix(const glm::mat4& m);

    /*
     *  Changes a variable's value
     */
    void setVar(Cache::VarId var, float value);

    /*
     *  Returns the current values associated with all variables
     */
    std::map<Cache::VarId, float> varValues() const;

    /*
     *  Updates variable values, returning true if changed
     */
    bool updateVars(const Cache& cache);

    /*
     *  Updates variable values, return true if changed
     */
    bool updateVars(const EvaluatorBase& other);

protected:
    /*
     *  Evaluate a single clause, populating the out array
     */
    static void eval_clause_values(Opcode::Opcode op,
        const float* __restrict a, const float* __restrict b,
        float* __restrict out, Result::Index count);

    /*
     *  Evaluate a set of derivatives (X, Y, Z)
     */
    static void eval_clause_derivs(Opcode::Opcode op,
        const float* __restrict av,  const float* __restrict adx,
        const float* __restrict ady, const float* __restrict adz,

        const float* __restrict bv,  const float* __restrict bdx,
        const float* __restrict bdy, const float* __restrict bdz,

        float* __restrict ov,  float* __restrict odx,
        float* __restrict ody, float* __restrict odz,
        Result::Index count);

    /*
     *  Evaluate the tree's values and Jacobian
     *  with respect to all its variables
     */
    static float eval_clause_jacobians(Opcode::Opcode op,
        const float av,  std::vector<float>& aj,
        const float bv,  std::vector<float>& bj,
                         std::vector<float>& oj);

    /*
     *  Evaluates a single Interval clause
     */
    static Interval eval_clause_interval(
        Opcode::Opcode op, const Interval& a, const Interval& b);

    /*  Global matrix transform (and inverse) applied to all coordinates  */
    glm::mat4 M;
    glm::mat4 Mi;

    /*  Indices of X, Y, Z coordinates */
    Clause::Id X, Y, Z;

    /*  Map of variables (in terms of where they live in this Evaluator) to
     *  their ids in their respective Tree (e.g. what you get when calling
     *  Tree::var(3.0).var() */
    boost::bimap<Clause::Id, Cache::VarId> vars;

    /*  Tape containing our opcodes in reverse order */
    typedef std::vector<Clause> Tape;
    std::list<Tape> tapes;
    std::list<Tape>::iterator tape;

    /*  Store the root opcode explicitly so that we can convert back into
     *  a tree even if there's nothing in the tape  */
    Opcode::Opcode root_op;

    std::vector<uint8_t> disabled;

    Result result;
};

}   // namespace Kernel
