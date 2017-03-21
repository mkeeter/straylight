#include <numeric>
#include <memory>
#include <cmath>

#include <glm/gtc/matrix_inverse.hpp>

#include "kernel/tree/cache.hpp"
#include "kernel/tree/tree.hpp"
#include "kernel/eval/evaluator_base.hpp"
#include "kernel/eval/clause.hpp"

namespace Kernel {

////////////////////////////////////////////////////////////////////////////////

EvaluatorBase::EvaluatorBase(const Tree root, const glm::mat4& M,
                             const std::map<Tree::Id, float>& vs)
    : root_op(root->op)
{
    setMatrix(M);

    auto flat = root.ordered();

    // Helper function to create a new clause in the data array
    // The dummy clause (0) is mapped to the first result slot
    std::unordered_map<Tree::Id, Clause::Id> clauses = {{nullptr, 0}};
    Clause::Id id = flat.size() - 1;

    // Helper function to make a new function
    std::list<Clause> tape_;
    auto newClause = [&clauses, &id, &tape_](const Tree::Id t)
    {
        tape_.push_front(
                {t->op,
                 id,
                 clauses.at(t->lhs.get()),
                 clauses.at(t->rhs.get())});
    };

    // Write the flattened tree into the tape!
    std::map<Clause::Id, float> constants;
    for (const auto& m : flat)
    {
        // Normal clauses end up in the tape
        if (m->rank > 0)
        {
            newClause(m.id());
        }
        // For constants and variables, record their values so
        // that we can store those values in the result array
        else if (m->op == Opcode::CONST)
        {
            constants[id] = m->value;
        }
        else if (m->op == Opcode::VAR)
        {
            constants[id] = vs.at(m.id());
            vars.left.insert({id, m.id()});
            var_handles.insert({m.id(), m});
        }
        else
        {
            assert(m->op == Opcode::VAR_X ||
                   m->op == Opcode::VAR_Y ||
                   m->op == Opcode::VAR_Z);
        }
        clauses[m.id()] = id--;
    }
    assert(id + 1 == 0);

    //  Move from the list tape to a more-compact vector tape
    tapes.push_back(Tape());
    tape = tapes.begin();
    for (auto& t : tape_)
    {
        tape->push_back(t);
    }

    // Make sure that X, Y, Z have been allocated space
    std::vector<Tree> axes = {Tree::X(), Tree::Y(), Tree::Z()};
    for (auto a : axes)
    {
        if (clauses.find(a.id()) == clauses.end())
        {
            clauses[a.id()] = clauses.size();
        }
    }

    // Allocate enough memory for all the clauses
    result.resize(clauses.size(), vars.size());
    disabled.resize(clauses.size());

    // Store all constants in results array
    for (auto c : constants)
    {
        result.fill(c.second, c.first);
    }

    // Save X, Y, Z ids
    X = clauses.at(axes[0].id());
    Y = clauses.at(axes[1].id());
    Z = clauses.at(axes[2].id());

    // Set derivatives for X, Y, Z (unchanging)
    result.setDeriv(1, 0, 0, X);
    result.setDeriv(0, 1, 0, Y);
    result.setDeriv(0, 0, 1, Z);

    {   // Set the Jacobian for our variables (unchanging)
        size_t index = 0;
        for (auto v : vars.left)
        {
            result.setGradient(v.first, index++);
        }
    }

    assert(clauses.at(root.id()) == 0);
}

////////////////////////////////////////////////////////////////////////////////

float EvaluatorBase::eval(float x, float y, float z)
{
    set(x, y, z, 0);
    return values(1)[0];
}

Interval EvaluatorBase::eval(Interval x, Interval y, Interval z)
{
    set(x, y, z);
    return interval();
}

void EvaluatorBase::set(Interval x, Interval y, Interval z)
{
    result.i[X] = M[0][0] * x + M[1][0] * y + M[2][0] * z + M[3][0];
    result.i[Y] = M[0][1] * x + M[1][1] * y + M[2][1] * z + M[3][1];
    result.i[Z] = M[0][2] * x + M[1][2] * y + M[2][2] * z + M[3][2];
}

////////////////////////////////////////////////////////////////////////////////

std::list<EvaluatorBase::Tape>::iterator EvaluatorBase::pushTape()
{
    auto current_tape = tape;

    // Add another tape to the top of the tape stack if one doesn't already
    // exist (we never erase them, to avoid re-allocating memory during
    // nested evaluations).
    if (++tape == tapes.end())
    {
        tape = tapes.insert(tape, Tape());
        tape->reserve(tapes.front().size());
    }
    else
    {
        // We may be reusing an existing tape, so resize to 0
        // (preserving allocated storage)
        tape->clear();
    }

    assert(tape != tapes.end());
    assert(tape->capacity() >= current_tape->size());
    return current_tape;
}

void EvaluatorBase::push()
{
    auto current_tape = pushTape();

    // Next, we figure out which clauses are disabled and only push enabled
    // clauses into the new tape (aspirationally)
    std::fill(disabled.begin(), disabled.end(), true);

    // Mark the root node as active
    disabled[0] = false;

    for (const auto& c : *current_tape)
    {
        if (!disabled[c.id])
        {
            // For min and max operations, we may only need to keep one branch
            // active if it is decisively above or below the other branch.
            if (c.op == Opcode::MAX)
            {
                if (result.i[c.a].lower() >= result.i[c.b].upper())
                {
                    disabled[c.a] = false;
                    tape->push_back({Opcode::DUMMY_A, c.id, c.a, 0});
                    continue;
                }
                else if (result.i[c.b].lower() >= result.i[c.a].upper())
                {
                    disabled[c.b] = false;
                    tape->push_back({Opcode::DUMMY_B, c.id, 0, c.b});
                    continue;
                }
            }
            else if (c.op == Opcode::MIN)
            {
                if (result.i[c.a].lower() >= result.i[c.b].upper())
                {
                    disabled[c.b] = false;
                    tape->push_back({Opcode::DUMMY_B, c.id, 0, c.b});
                    continue;
                }
                else if (result.i[c.b].lower() >= result.i[c.a].upper())
                {
                    disabled[c.a] = false;
                    tape->push_back({Opcode::DUMMY_A, c.id, c.a, 0});
                    continue;
                }
            }
            disabled[c.a] = false;
            disabled[c.b] = false;
            tape->push_back(c);
        }
    }
    assert(tape->size() <= current_tape->size());
}

void EvaluatorBase::specialize(float x, float y, float z)
{
    // Load results into the first floating-point result slot
    eval(x, y, z);

    // The same logic as push, but using float instead of interval comparisons
    auto current_tape = pushTape();

    // Next, we figure out which clauses are disabled and only push enabled
    // clauses into the new tape (aspirationally)
    std::fill(disabled.begin(), disabled.end(), true);

    // Mark the root node as active
    disabled[0] = false;

    for (const auto& c : *current_tape)
    {
        if (!disabled[c.id])
        {
            // For min and max operations, we may only need to keep one branch
            // active if it is decisively above or below the other branch.
            if (c.op == Opcode::MAX)
            {
                if (result.f[c.a][0] >= result.f[c.b][0])
                {
                    disabled[c.a] = false;
                    tape->push_back({Opcode::DUMMY_A, c.id, c.a, 0});
                    continue;
                }
                else if (result.f[c.b][0] >= result.f[c.a][0])
                {
                    disabled[c.b] = false;
                    tape->push_back({Opcode::DUMMY_B, c.id, 0, c.b});
                    continue;
                }
            }
            else if (c.op == Opcode::MIN)
            {
                if (result.f[c.a][0] >= result.f[c.b][0])
                {
                    disabled[c.b] = false;
                    tape->push_back({Opcode::DUMMY_B, c.id, 0, c.b});
                    continue;
                }
                else if (result.f[c.b][0] >= result.f[c.a][0])
                {
                    disabled[c.a] = false;
                    tape->push_back({Opcode::DUMMY_A, c.id, c.a, 0});
                    continue;
                }
            }
            disabled[c.a] = false;
            disabled[c.b] = false;
            tape->push_back(c);
        }
    }
    assert(tape->size() <= current_tape->size());
}

void EvaluatorBase::pop()
{
    assert(tape != tapes.begin());
    tape--;
}

////////////////////////////////////////////////////////////////////////////////

#define EVAL_LOOP for (Result::Index i=0; i < count; ++i)
void EvaluatorBase::eval_clause_values(Opcode::Opcode op,
        const float* __restrict a, const float* __restrict b,
        float* __restrict out, Result::Index count)
{
    switch (op) {
        case Opcode::ADD:
            EVAL_LOOP
            out[i] = a[i] + b[i];
            break;
        case Opcode::MUL:
            EVAL_LOOP
            out[i] = a[i] * b[i];
            break;
        case Opcode::MIN:
            EVAL_LOOP
            out[i] = fmin(a[i], b[i]);
            break;
        case Opcode::MAX:
            EVAL_LOOP
            out[i] = fmax(a[i], b[i]);
            break;
        case Opcode::SUB:
            EVAL_LOOP
            out[i] = a[i] - b[i];
            break;
        case Opcode::DIV:
            EVAL_LOOP
            out[i] = a[i] / b[i];
            break;
        case Opcode::ATAN2:
            EVAL_LOOP
            out[i] = atan2(a[i], b[i]);
            break;
        case Opcode::POW:
            EVAL_LOOP
            out[i] = pow(a[i], b[i]);
            break;
        case Opcode::NTH_ROOT:
            EVAL_LOOP
            out[i] = pow(a[i], 1.0f/b[i]);
            break;
        case Opcode::MOD:
            EVAL_LOOP
            {
                out[i] = std::fmod(a[i], b[i]);
                while (out[i] < 0)
                {
                    out[i] += b[i];
                }
            }
            break;
        case Opcode::NANFILL:
            EVAL_LOOP
            out[i] = std::isnan(a[i]) ? b[i] : a[i];
            break;

        case Opcode::SQUARE:
            EVAL_LOOP
            out[i] = a[i] * a[i];
            break;
        case Opcode::SQRT:
            EVAL_LOOP
            out[i] = sqrt(a[i]);
            break;
        case Opcode::NEG:
            EVAL_LOOP
            out[i] = -a[i];
            break;
        case Opcode::ABS:
            EVAL_LOOP
            out[i] = fabs(a[i]);
            break;
        case Opcode::SIN:
            EVAL_LOOP
            out[i] = sin(a[i]);
            break;
        case Opcode::COS:
            EVAL_LOOP
            out[i] = cos(a[i]);
            break;
        case Opcode::TAN:
            EVAL_LOOP
            out[i] = tan(a[i]);
            break;
        case Opcode::ASIN:
            EVAL_LOOP
            out[i] = asin(a[i]);
            break;
        case Opcode::ACOS:
            EVAL_LOOP
            out[i] = acos(a[i]);
            break;
        case Opcode::ATAN:
            EVAL_LOOP
            out[i] = atan(a[i]);
            break;
        case Opcode::EXP:
            EVAL_LOOP
            out[i] = exp(a[i]);
            break;

        case Opcode::DUMMY_A:
            EVAL_LOOP
            out[i] = a[i];
            break;
        case Opcode::DUMMY_B:
            EVAL_LOOP
            out[i] = b[i];
            break;
        case Opcode::CONST_VAR:
            EVAL_LOOP
            out[i] = a[i];
            break;

        case Opcode::INVALID:
        case Opcode::CONST:
        case Opcode::VAR_X:
        case Opcode::VAR_Y:
        case Opcode::VAR_Z:
        case Opcode::VAR:
        case Opcode::LAST_OP: assert(false);
    }
}

void EvaluatorBase::eval_clause_derivs(Opcode::Opcode op,
        const float* __restrict av,  const float* __restrict adx,
        const float* __restrict ady, const float* __restrict adz,

        const float* __restrict bv,  const float* __restrict bdx,
        const float* __restrict bdy, const float* __restrict bdz,

        float* __restrict ov,  float* __restrict odx,
        float* __restrict ody, float* __restrict odz,
        Result::Index count)
{
    // Evaluate the base operations in a single pass
    eval_clause_values(op, av, bv, ov, count);

    switch (op) {
        case Opcode::ADD:
            EVAL_LOOP
            {
                odx[i] = adx[i] + bdx[i];
                ody[i] = ady[i] + bdy[i];
                odz[i] = adz[i] + bdz[i];
            }
            break;
        case Opcode::MUL:
            EVAL_LOOP
            {   // Product rule
                odx[i] = av[i]*bdx[i] + adx[i]*bv[i];
                ody[i] = av[i]*bdy[i] + ady[i]*bv[i];
                odz[i] = av[i]*bdz[i] + adz[i]*bv[i];
            }
            break;
        case Opcode::MIN:
            EVAL_LOOP
            {
                if (av[i] < bv[i])
                {
                    odx[i] = adx[i];
                    ody[i] = ady[i];
                    odz[i] = adz[i];
                }
                else
                {
                    odx[i] = bdx[i];
                    ody[i] = bdy[i];
                    odz[i] = bdz[i];
                }
            }
            break;
        case Opcode::MAX:
            EVAL_LOOP
            {
                if (av[i] < bv[i])
                {
                    odx[i] = bdx[i];
                    ody[i] = bdy[i];
                    odz[i] = bdz[i];
                }
                else
                {
                    odx[i] = adx[i];
                    ody[i] = ady[i];
                    odz[i] = adz[i];
                }
            }
            break;
        case Opcode::SUB:
            EVAL_LOOP
            {
                odx[i] = adx[i] - bdx[i];
                ody[i] = ady[i] - bdy[i];
                odz[i] = adz[i] - bdz[i];
            }
            break;
        case Opcode::DIV:
            EVAL_LOOP
            {
                const float p = pow(bv[i], 2);
                odx[i] = (bv[i]*adx[i] - av[i]*bdx[i]) / p;
                ody[i] = (bv[i]*ady[i] - av[i]*bdy[i]) / p;
                odz[i] = (bv[i]*adz[i] - av[i]*bdz[i]) / p;
            }
            break;
        case Opcode::ATAN2:
            EVAL_LOOP
            {
                const float d = pow(av[i], 2) + pow(bv[i], 2);
                odx[i] = (adx[i]*bv[i] - av[i]*bdx[i]) / d;
                ody[i] = (ady[i]*bv[i] - av[i]*bdy[i]) / d;
                odz[i] = (adz[i]*bv[i] - av[i]*bdz[i]) / d;
            }
            break;
        case Opcode::POW:
            EVAL_LOOP
            {
                const float m = pow(av[i], bv[i] - 1);

                // The full form of the derivative is
                // odx[i] = m * (bv[i] * adx[i] + av[i] * log(av[i]) * bdx[i]))
                // However, log(av[i]) is often NaN and bdx[i] is always zero,
                // (since it must be CONST), so we skip that part.
                odx[i] = m * (bv[i] * adx[i]);
                ody[i] = m * (bv[i] * ady[i]);
                odz[i] = m * (bv[i] * adz[i]);
            }
            break;
        case Opcode::NTH_ROOT:
            EVAL_LOOP
            {
                const float m = pow(av[i], 1.0f/bv[i] - 1);
                odx[i] = m * (1.0f/bv[i] * adx[i]);
                ody[i] = m * (1.0f/bv[i] * ady[i]);
                odz[i] = m * (1.0f/bv[i] * adz[i]);
            }
            break;
        case Opcode::MOD:
            EVAL_LOOP
            {
                // This isn't quite how partial derivatives of mod work,
                // but close enough normals rendering.
                odx[i] = adx[i];
                ody[i] = ady[i];
                odz[i] = adz[i];
            }
            break;
        case Opcode::NANFILL:
            EVAL_LOOP
            {
                odx[i] = std::isnan(av[i]) ? bdx[i] : adx[i];
                ody[i] = std::isnan(av[i]) ? bdy[i] : ady[i];
                odz[i] = std::isnan(av[i]) ? bdz[i] : adz[i];
            }
            break;

        case Opcode::SQUARE:
            EVAL_LOOP
            {
                odx[i] = 2 * av[i] * adx[i];
                ody[i] = 2 * av[i] * ady[i];
                odz[i] = 2 * av[i] * adz[i];
            }
            break;
        case Opcode::SQRT:
            EVAL_LOOP
            {
                if (av[i] < 0)
                {
                    odx[i] = 0;
                    ody[i] = 0;
                    odz[i] = 0;
                }
                else
                {
                    odx[i] = adx[i] / (2 * ov[i]);
                    ody[i] = ady[i] / (2 * ov[i]);
                    odz[i] = adz[i] / (2 * ov[i]);
                }
            }
            break;
        case Opcode::NEG:
            EVAL_LOOP
            {
                odx[i] = -adx[i];
                ody[i] = -ady[i];
                odz[i] = -adz[i];
            }
            break;
        case Opcode::ABS:
            EVAL_LOOP
            {
                if (av[i] < 0)
                {
                    odx[i] = -adx[i];
                    ody[i] = -ady[i];
                    odz[i] = -adz[i];
                }
                else
                {
                    odx[i] = adx[i];
                    ody[i] = ady[i];
                    odz[i] = adz[i];
                }
            }
            break;
        case Opcode::SIN:
            EVAL_LOOP
            {
                const float c = cos(av[i]);
                odx[i] = adx[i] * c;
                ody[i] = ady[i] * c;
                odz[i] = adz[i] * c;
            }
            break;
        case Opcode::COS:
            EVAL_LOOP
            {
                const float s = -sin(av[i]);
                odx[i] = adx[i] * s;
                ody[i] = ady[i] * s;
                odz[i] = adz[i] * s;
            }
            break;
        case Opcode::TAN:
            EVAL_LOOP
            {
                const float s = pow(1/cos(av[i]), 2);
                odx[i] = adx[i] * s;
                ody[i] = ady[i] * s;
                odz[i] = adz[i] * s;
            }
            break;
        case Opcode::ASIN:
            EVAL_LOOP
            {
                const float d = sqrt(1 - pow(av[i], 2));
                odx[i] = adx[i] / d;
                ody[i] = ady[i] / d;
                odz[i] = adz[i] / d;
            }
            break;
        case Opcode::ACOS:
            EVAL_LOOP
            {
                const float d = -sqrt(1 - pow(av[i], 2));
                odx[i] = adx[i] / d;
                ody[i] = ady[i] / d;
                odz[i] = adz[i] / d;
            }
            break;
        case Opcode::ATAN:
            EVAL_LOOP
            {
                const float d = pow(av[i], 2) + 1;
                odx[i] = adx[i] / d;
                ody[i] = ady[i] / d;
                odz[i] = adz[i] / d;
            }
            break;
        case Opcode::EXP:
            EVAL_LOOP
            {
                const float e = exp(av[i]);
                odx[i] = e * adx[i];
                ody[i] = e * ady[i];
                odz[i] = e * adz[i];
            }
            break;

        case Opcode::DUMMY_A:
            EVAL_LOOP
            {
                odx[i] = adx[i];
                ody[i] = ady[i];
                odz[i] = adz[i];
            }
            break;
        case Opcode::DUMMY_B:
            EVAL_LOOP
            {
                odx[i] = bdx[i];
                ody[i] = bdy[i];
                odz[i] = bdz[i];
            }
            break;
        case Opcode::CONST_VAR:
            EVAL_LOOP
            {
                odx[i] = adx[i];
                ody[i] = ady[i];
                odz[i] = adz[i];
            }
            break;

        case Opcode::INVALID:
        case Opcode::CONST:
        case Opcode::VAR_X:
        case Opcode::VAR_Y:
        case Opcode::VAR_Z:
        case Opcode::VAR:
        case Opcode::LAST_OP: assert(false);
    }
}

#define JAC_LOOP for (auto a = aj.begin(), b = bj.begin(), o = oj.begin(); a != aj.end(); ++a, ++b, ++o)
float EvaluatorBase::eval_clause_jacobians(Opcode::Opcode op,
        const float av,  std::vector<float>& aj,
        const float bv,  std::vector<float>& bj,
        std::vector<float>& oj)
{
    // Evaluate the base operations in a single pass
    float out;
    eval_clause_values(op, &av, &bv, &out, 1);

    switch (op) {
        case Opcode::ADD:
            JAC_LOOP
            {
                (*o) = (*a) + (*b);
            }
            break;
        case Opcode::MUL:
            JAC_LOOP
            {   // Product rule
                (*o) = av * (*b) + bv * (*a);
            }
            break;
        case Opcode::MIN:
            JAC_LOOP
            {
                if (av < bv)
                {
                    (*o) = (*a);
                }
                else
                {
                    (*o) = (*b);
                }
            }
            break;
        case Opcode::MAX:
            JAC_LOOP
            {
                if (av < bv)
                {
                    (*o) = (*b);
                }
                else
                {
                    (*o) = (*a);
                }
            }
            break;
        case Opcode::SUB:
            JAC_LOOP
            {
                (*o) = (*a) - (*b);
            }
            break;
        case Opcode::DIV:
            JAC_LOOP
            {
                const float p = pow(bv, 2);
                (*o) = (bv*(*a) - av*(*b)) / p;
            }
            break;
        case Opcode::ATAN2:
            JAC_LOOP
            {
                const float d = pow(av, 2) + pow(bv, 2);
                (*o) = ((*a)*bv - av*(*b)) / d;
            }
            break;
        case Opcode::POW:
            JAC_LOOP
            {
                const float m = pow(av, bv - 1);

                // The full form of the derivative is
                // (*o) = m * (bv * (*a) + av * log(av) * (*b)))
                // However, log(av) is often NaN and (*b) is always zero,
                // (since it must be CONST), so we skip that part.
                (*o) = m * (bv * (*a));
            }
            break;
        case Opcode::NTH_ROOT:
            JAC_LOOP
            {
                const float m = pow(av, 1.0f/bv - 1);
                (*o) = m * (1.0f/bv * (*a));
            }
            break;
        case Opcode::MOD:
            JAC_LOOP
            {
                // This isn't quite how partial derivatives of mod work,
                // but close enough normals rendering.
                (*o) = (*a);
            }
            break;
        case Opcode::NANFILL:
            JAC_LOOP
            {
                (*o) = std::isnan(av) ? (*b) : (*a);
            }
            break;

        case Opcode::SQUARE:
            JAC_LOOP
            {
                (*o) = 2 * av * (*a);
            }
            break;
        case Opcode::SQRT:
            JAC_LOOP
            {
                if (av < 0)
                {
                    (*o) = 0;
                }
                else
                {
                    (*o) = (*a) / (2 * out);
                }
            }
            break;
        case Opcode::NEG:
            JAC_LOOP
            {
                (*o) = -(*a);
            }
            break;
        case Opcode::ABS:
            JAC_LOOP
            {
                if (av < 0)
                {
                    (*o) = -(*a);
                }
                else
                {
                    (*o) = (*a);
                }
            }
            break;
        case Opcode::SIN:
            JAC_LOOP
            {
                const float c = cos(av);
                (*o) = (*a) * c;
            }
            break;
        case Opcode::COS:
            JAC_LOOP
            {
                const float s = -sin(av);
                (*o) = (*a) * s;
            }
            break;
        case Opcode::TAN:
            JAC_LOOP
            {
                const float s = pow(1/cos(av), 2);
                (*o) = (*a) * s;
            }
            break;
        case Opcode::ASIN:
            JAC_LOOP
            {
                const float d = sqrt(1 - pow(av, 2));
                (*o) = (*a) / d;
            }
            break;
        case Opcode::ACOS:
            JAC_LOOP
            {
                const float d = -sqrt(1 - pow(av, 2));
                (*o) = (*a) / d;
            }
            break;
        case Opcode::ATAN:
            JAC_LOOP
            {
                const float d = pow(av, 2) + 1;
                (*o) = (*a) / d;
            }
            break;
        case Opcode::EXP:
            JAC_LOOP
            {
                const float e = exp(av);
                (*o) = e * (*a);
            }
            break;

        case Opcode::DUMMY_A:
            JAC_LOOP
            {
                (*o) = (*a);
            }
            break;
        case Opcode::DUMMY_B:
            JAC_LOOP
            {
                (*o) = (*b);
            }
            break;
        case Opcode::CONST_VAR:
            JAC_LOOP
            {
                (*o) = 0;
            }
            break;

        case Opcode::INVALID:
        case Opcode::CONST:
        case Opcode::VAR_X:
        case Opcode::VAR_Y:
        case Opcode::VAR_Z:
        case Opcode::VAR:
        case Opcode::LAST_OP: assert(false);
    }

    return out;
}

Interval EvaluatorBase::eval_clause_interval(
        Opcode::Opcode op, const Interval& a, const Interval& b)
{
    switch (op) {
        case Opcode::ADD:
            return a + b;
        case Opcode::MUL:
            return a * b;
        case Opcode::MIN:
            return boost::numeric::min(a, b);
        case Opcode::MAX:
            return boost::numeric::max(a, b);
        case Opcode::SUB:
            return a - b;
        case Opcode::DIV:
            return a / b;
        case Opcode::ATAN2:
            return atan2(a, b);
        case Opcode::POW:
            return boost::numeric::pow(a, b.lower());
        case Opcode::NTH_ROOT:
            return boost::numeric::nth_root(a, b.lower());
        case Opcode::MOD:
            return Interval(0.0f, b.upper()); // YOLO
        case Opcode::NANFILL:
            return (std::isnan(a.lower()) || std::isnan(a.upper())) ? b : a;

        case Opcode::SQUARE:
            return boost::numeric::square(a);
        case Opcode::SQRT:
            return boost::numeric::sqrt(a);
        case Opcode::NEG:
            return -a;
        case Opcode::ABS:
            return boost::numeric::abs(a);
        case Opcode::SIN:
            return boost::numeric::sin(a);
        case Opcode::COS:
            return boost::numeric::cos(a);
        case Opcode::TAN:
            return boost::numeric::tan(a);
        case Opcode::ASIN:
            return boost::numeric::asin(a);
        case Opcode::ACOS:
            return boost::numeric::acos(a);
        case Opcode::ATAN:
            return boost::numeric::atan(a);
        case Opcode::EXP:
            return boost::numeric::exp(a);

        case Opcode::DUMMY_A:
            return a;
        case Opcode::DUMMY_B:
            return b;
        case Opcode::CONST_VAR:
            return a;

        case Opcode::INVALID:
        case Opcode::CONST:
        case Opcode::VAR_X:
        case Opcode::VAR_Y:
        case Opcode::VAR_Z:
        case Opcode::VAR:
        case Opcode::LAST_OP: assert(false);
    }
    return Interval();
}

////////////////////////////////////////////////////////////////////////////////

const float* EvaluatorBase::values(Result::Index count)
{
    for (auto itr = tape->rbegin(); itr != tape->rend(); ++itr)
    {
        eval_clause_values(itr->op,
                &result.f[itr->a][0], &result.f[itr->b][0],
                &result.f[itr->id][0], count);
    }

    return &result.f[0][0];
}

EvaluatorBase::Derivs EvaluatorBase::derivs(Result::Index count)
{
    for (auto itr = tape->rbegin(); itr != tape->rend(); ++itr)
    {
        eval_clause_derivs(itr->op,
               &result.f[itr->a][0], &result.dx[itr->a][0],
               &result.dy[itr->a][0], &result.dz[itr->a][0],

               &result.f[itr->b][0], &result.dx[itr->b][0],
               &result.dy[itr->b][0], &result.dz[itr->b][0],

               &result.f[itr->id][0], &result.dx[itr->id][0],
               &result.dy[itr->id][0], &result.dz[itr->id][0],
               count);
    }

    // Apply the inverse matrix transform to our normals
    auto o = Mi * glm::vec4(0,0,0,1);
    for (size_t i=0; i < count; ++i)
    {
        auto n = Mi * glm::vec4(result.dx[0][i],
                                result.dy[0][i],
                                result.dz[0][i], 1) - o;
        result.dx[0][i] = n.x;
        result.dy[0][i] = n.y;
        result.dz[0][i] = n.z;
    }

    return { &result.f[0][0], &result.dx[0][0],
             &result.dy[0][0], &result.dz[0][0] };
}

std::map<Tree::Id, float> EvaluatorBase::gradient(float x, float y, float z)
{
    set(x, y, z, 0);

    for (auto itr = tape->rbegin(); itr != tape->rend(); ++itr)
    {
        float av = result.f[itr->a][0];
        float bv = result.f[itr->b][0];
        std::vector<float>& aj = result.j[itr->a];
        std::vector<float>& bj = result.j[itr->b];

        result.f[itr->id][0] = eval_clause_jacobians(
                itr->op, av, aj, bv, bj, result.j[itr->id]);
    }

    std::map<Tree::Id, float> out;
    {   // Unpack from flat array into map
        // (to allow correlating back to VARs in Tree)
        size_t index = 0;
        for (auto v : vars.left)
        {
            out[v.second] = result.j[0][index++];
        }
    }
    return out;
}

Interval EvaluatorBase::interval()
{
    for (auto itr = tape->rbegin(); itr != tape->rend(); ++itr)
    {
        Interval a = result.i[itr->a];
        Interval b = result.i[itr->b];

        result.i[itr->id] = eval_clause_interval(itr->op, a, b);
    }
    return result.i[0];
}

////////////////////////////////////////////////////////////////////////////////

void EvaluatorBase::applyTransform(Result::Index count)
{
    for (size_t i=0; i < count; ++i)
    {
        float x = result.f[X][i];
        float y = result.f[Y][i];
        float z = result.f[Z][i];
        result.f[X][i] = M[0][0] * x + M[1][0] * y + M[2][0] * z + M[3][0];
        result.f[Y][i] = M[0][1] * x + M[1][1] * y + M[2][1] * z + M[3][1];
        result.f[Z][i] = M[0][2] * x + M[1][2] * y + M[2][2] * z + M[3][2];
    }
}

////////////////////////////////////////////////////////////////////////////////

double EvaluatorBase::utilization() const
{
    return tape->size() / double(tapes.front().size());
}

void EvaluatorBase::setMatrix(const glm::mat4& m)
{
    M = m;
    Mi = glm::inverse(m);
}

void EvaluatorBase::setVar(Tree::Id var, float value)
{
    auto r = vars.right.find(var);
    if (r != vars.right.end())
    {
        result.setValue(value, r->second);
    }
}

std::map<Tree::Id, float> EvaluatorBase::varValues() const
{
    std::map<Tree::Id, float> out;

    for (auto v : vars.left)
    {
        out[v.second] = result.f[v.first][0];
    }
    return out;
}

bool EvaluatorBase::updateVars(const std::map<Kernel::Tree::Id, float>& vars_)
{
    bool changed = false;
    for (const auto& v : vars.left)
    {
        auto val = vars_.at(v.second);
        if (val != result.f[v.first][0])
        {
            setVar(v.second, val);
            changed = true;
        }
    }
    return changed;
}

}   // namespace Kernel
