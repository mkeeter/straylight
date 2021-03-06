#include "s7/s7.h"

#include "kernel/bind/bind_s7.h"
#include "kernel/eval/evaluator.hpp"

namespace Kernel {
namespace Bind {

// Populated in bind_s7
int shape_t::tag = -1;

////////////////////////////////////////////////////////////////////////////////

float shape_t::value() const
{
    return Kernel::Evaluator(tree, vars).values(1)[0];
}

bool is_shape(s7_pointer s)
{
    return s7_is_object(s) && s7_object_type(s) == shape_t::tag;
}

static s7_pointer is_shape(s7_scheme *sc, s7_pointer args)
{
    return s7_make_boolean(sc, is_shape(s7_car(args)));
}

const shape_t* get_shape(s7_pointer obj)
{
    return static_cast<shape_t*>(s7_object_value_checked(obj, shape_t::tag));
}

static shape_t* get_mutable_shape(s7_pointer obj)
{
    return static_cast<shape_t*>(s7_object_value_checked(obj, shape_t::tag));
}

////////////////////////////////////////////////////////////////////////////////

/*
 *  Converts a Tree into an allocated Scheme shape
 */
s7_pointer shape_from_tree(s7_scheme* sc, Kernel::Tree t,
                           std::map<Kernel::Tree::Id, float> vars)
{
    auto s = new shape_t(t);
    s->vars = vars;
    return s7_make_object(sc, shape_t::tag, s);
}

/*
 *  Shape constructor from arbitrary Scheme object
 *
 *  On failure, returns an error message with the given function name
 */
s7_pointer shape_from_obj(s7_scheme* sc, s7_pointer obj, const char* func_name,
                          bool throw_error)
{
    if (is_shape(obj))
    {
        return obj;
    }
    else if (s7_is_number(obj))
    {
        return shape_from_tree(sc, Kernel::Tree(s7_number_to_real(sc, obj)));
    }
    else if (throw_error)
    {
        return s7_wrong_type_arg_error(sc, func_name, 0, obj,
                "shape or real");
    }
    else
    {
        return obj;
    }
}

/*
 *  Shape constructor from an arbitrary function object
 */
static s7_pointer shape_from_lambda(s7_scheme* sc, s7_pointer args)
{
    //  Call the shape, which either builds up the tree (if the argument is a
    //  lambda) or remaps the shape (if it was already a shape).
    auto obj = s7_call(sc, s7_car(args), s7_list(sc, 3,
            shape_from_tree(sc, Kernel::Tree::X()),
            shape_from_tree(sc, Kernel::Tree::Y()),
            shape_from_tree(sc, Kernel::Tree::Z())));

    // If it autocollapsed into a constant, then wrap that in a constant Tree
    // Errors are throw in shape_apply, so don't rethrow here
    return shape_from_obj(sc, obj, "shape_from_lambda", false);
}

#define CHECK_SHAPE(OBJ) {   if (!is_shape(OBJ)) return OBJ; }

// If the result is a constant (indicating that there was no shape_t
// involved in the reduction), then convert back to an ordinary
// Scheme real to keep arithmetic working as usual.
static s7_pointer shape_check_const(s7_scheme* sc, s7_pointer out)
{
    CHECK_SHAPE(out);

    auto& tree = get_shape(out)->tree;

    return (tree->op == Kernel::Opcode::CONST)
        ? s7_make_real(sc, tree->value) : out;
}

////////////////////////////////////////////////////////////////////////////////

s7_pointer shape_unary(s7_scheme* sc, Kernel::Opcode::Opcode op,
                       s7_pointer obj, const char* func_name="shape_unary")
{
    auto s = shape_from_obj(sc, obj, func_name);
    CHECK_SHAPE(s);

    return shape_check_const(sc, shape_from_tree(sc,
                Kernel::Tree(op, get_shape(s)->tree),
                get_shape(s)->vars));
}

s7_pointer shape_binary(s7_scheme* sc, Kernel::Opcode::Opcode op,
                        s7_pointer lhs, s7_pointer rhs,
                        const char* func_name="shape_binary")
{
    auto a = shape_from_obj(sc, lhs, func_name);
    CHECK_SHAPE(a);
    auto b = shape_from_obj(sc, rhs, func_name);
    CHECK_SHAPE(b);

    // Build subtraction operation
    auto out = shape_from_tree(sc,
                Kernel::Tree(op, get_shape(a)->tree, get_shape(b)->tree),
                get_shape(a)->vars);

    // Insert all variable bindings from rhs
    get_mutable_shape(out)->vars.insert(get_shape(b)->vars.begin(),
                                        get_shape(b)->vars.end());
    return shape_check_const(sc, out);

}

////////////////////////////////////////////////////////////////////////////////

static void shape_free(void* s)
{
    delete static_cast<shape_t*>(s);
}

static bool shape_equal(void* a, void* b)
{
    auto sa = static_cast<shape_t*>(a);
    auto sb = static_cast<shape_t*>(b);

    if (!(sa->tree == sb->tree))
    {
        return false;
    }
    else
    {
        const auto& va = sa->vars;
        const auto& vb = sb->vars;

        // If these are the same Tree, then they must have the same
        // set of variables
        assert(va.size() == vb.size());

        for (auto& k : va)
        {
            if (vb.at(k.first) != k.second)
            {
                return false;
            }
        }
    }

    return true;
}

static char* shape_print(s7_scheme* sc, void* s)
{
    (void)sc;

    auto shape = static_cast<shape_t*>(s);
    std::stringstream ss;

    if (shape->tree->flags & Tree::FLAG_LOCATION_AGNOSTIC)
    {
        ss << shape->value();
    }
    else
    {
        ss << "#<shape>";
    }

    auto str = ss.str();

    auto out = static_cast<char*>(calloc(str.length() + 1, sizeof(char)));
    memcpy(out, &str[0], str.length());
    return out;
}

static s7_pointer shape_apply(s7_scheme* sc, s7_pointer obj, s7_pointer args)
{
    if (s7_list_length(sc, args) != 3)
    {
        return s7_wrong_number_of_args_error(sc, "apply-shape: wrong number of args: ~A", args);
    }
    assert(is_shape(obj));

    auto x = shape_from_obj(sc, s7_car(args), "shape-apply");
    CHECK_SHAPE(x);
    auto y = shape_from_obj(sc, s7_cadr(args), "shape-apply");
    CHECK_SHAPE(y);
    auto z = shape_from_obj(sc, s7_caddr(args), "shape-apply");
    CHECK_SHAPE(z);

    auto t = get_shape(obj)->tree.remap(
            get_shape(x)->tree, get_shape(y)->tree, get_shape(z)->tree);
    auto out = shape_from_tree(sc, t, get_shape(obj)->vars);

    // Load all variable bindings into the new shape
    for (auto& a : { x, y, z })
    {
        get_mutable_shape(out)->vars.insert(get_shape(a)->vars.begin(),
                                            get_shape(a)->vars.end());
    }
    return out;
}

////////////////////////////////////////////////////////////////////////////////
//  Operations

#define REDUCER_DEFAULT(NAME, FUNC, OP, DEFAULT) \
static s7_pointer reduce_##NAME(s7_scheme* sc, s7_pointer list) \
{                                                               \
    switch (s7_list_length(sc, list))                           \
    {                                                           \
        case 0:                                                 \
        {                                                       \
            /* On an empty list, return the default value */    \
            return shape_from_tree(sc, Kernel::Tree(DEFAULT));  \
        }                                                       \
        case 1:                                                 \
        {                                                       \
            return shape_from_obj(sc, s7_car(list), FUNC);      \
        }                                                       \
        default:                                                \
        {                                                       \
            return shape_binary(sc, OP, s7_car(list),           \
                    reduce_##NAME(sc, s7_cdr(list)),            \
                    FUNC);                                      \
        }                                                       \
    }                                                           \
}

#define REDUCER_NO_DEFAULT(NAME, FUNC, OP) \
static s7_pointer reduce_##NAME(s7_scheme* sc, s7_pointer list) \
{                                                               \
    if (s7_list_length(sc, list) == 1)                          \
    {                                                           \
        return shape_from_obj(sc, s7_car(list), FUNC);          \
    }                                                           \
    else                                                        \
    {                                                           \
        /*  Length-0 lists should be prevented by caller */     \
        return shape_binary(sc, OP, s7_car(list),               \
                reduce_##NAME(sc, s7_cdr(list)),                \
                FUNC);                                          \
    }                                                           \
}

////////////////////////////////////////////////////////////////////////////////

#define OVERLOAD_COMMUTATIVE(NAME, FUNC)                                    \
static s7_pointer shape_##NAME(s7_scheme* sc, s7_pointer args)              \
{                                                                           \
    return shape_check_const(sc, reduce_##NAME(sc, args));                  \
}

REDUCER_DEFAULT(add, "+", Kernel::Opcode::ADD, 0);
REDUCER_DEFAULT(mul, "*", Kernel::Opcode::MUL, 1);
REDUCER_DEFAULT(add_sub, "-", Kernel::Opcode::ADD, 0);
REDUCER_DEFAULT(mul_div, "/", Kernel::Opcode::MUL, 1);
REDUCER_NO_DEFAULT(min, "min", Kernel::Opcode::MIN);
REDUCER_NO_DEFAULT(max, "max", Kernel::Opcode::MAX);

OVERLOAD_COMMUTATIVE(add, "+");
OVERLOAD_COMMUTATIVE(mul, "*");
OVERLOAD_COMMUTATIVE(min, "min");
OVERLOAD_COMMUTATIVE(max, "max");

static s7_pointer shape_sub(s7_scheme* sc, s7_pointer args)
{
    if (s7_list_length(sc, args) == 1)
    {
        return shape_unary(sc, Kernel::Opcode::NEG, s7_car(args), "-");
    }
    else
    {
        return shape_binary(sc, Kernel::Opcode::SUB, s7_car(args),
                reduce_add_sub(sc, s7_cdr(args)));
    }
}

static s7_pointer shape_div(s7_scheme* sc, s7_pointer args)
{
    if (s7_list_length(sc, args) == 1)
    {
        return shape_binary(sc, Kernel::Opcode::DIV,
                            s7_make_real(sc, 1.0f), s7_car(args), "/");
    }
    else
    {
        return shape_binary(sc, Kernel::Opcode::DIV, s7_car(args),
                reduce_mul_div(sc, s7_cdr(args)));
    }
}

static s7_pointer shape_atan(s7_scheme* sc, s7_pointer args)
{
    if (s7_list_length(sc, args) == 1)
    {
        return shape_unary(sc, Kernel::Opcode::ATAN, s7_car(args), "atan");
    }
    else
    {
        return shape_binary(sc, Kernel::Opcode::ATAN2,
                            s7_car(args), s7_cadr(args), "atan");
    }
}

static s7_pointer shape_expt(s7_scheme* sc, s7_pointer args)
{
    auto a = s7_car(args);
    auto b = s7_cadr(args);

    if (s7_is_number(a) && s7_is_number(b))
    {
        return s7_make_real(sc, pow(s7_number_to_real(sc, a),
                                    s7_number_to_real(sc, b)));
    }
    else if (s7_is_integer(b))
    {
        return shape_binary(sc, Kernel::Opcode::POW, a, b, "expt");
    }
    else if (s7_is_ratio(b) && s7_numerator(b) == 1)
    {
        return shape_binary(sc, Kernel::Opcode::NTH_ROOT,
                            a, s7_make_real(sc, s7_denominator(b)),
                            "expt");
    }
    else
    {
        return s7_wrong_type_arg_error(sc, "expt", 2, b,
                "integer or 1/integer");
    }
}

static s7_pointer shape_modulo(s7_scheme* sc, s7_pointer args)
{
    return shape_binary(sc, Kernel::Opcode::MOD,
                        s7_car(args), s7_cadr(args), "modulo");
}

#define OVERLOAD_UNARY(FUNC)                                    \
static s7_pointer shape_##FUNC(s7_scheme* sc, s7_pointer args)  \
{                                                               \
    auto s = shape_from_obj(sc, s7_car(args), #FUNC);           \
    CHECK_SHAPE(s);                                             \
    return shape_check_const(sc, shape_from_tree(sc,            \
                FUNC(get_shape(s)->tree),                       \
                get_shape(s)->vars));                           \
}
OVERLOAD_UNARY(square);
OVERLOAD_UNARY(sqrt);
OVERLOAD_UNARY(abs);
OVERLOAD_UNARY(sin);
OVERLOAD_UNARY(cos);
OVERLOAD_UNARY(tan);
OVERLOAD_UNARY(asin);
OVERLOAD_UNARY(acos);
OVERLOAD_UNARY(exp);

////////////////////////////////////////////////////////////////////////////////

static void install_overload(s7_scheme* sc, const char* op, int required_args,
                             int optional_args, bool rest_args,
                             s7_pointer (*f)(s7_scheme*, s7_pointer))
{
    s7_define_function(sc, op, f, required_args, optional_args, rest_args,
                       s7_help(sc, s7_name_to_value(sc, op)));
}

////////////////////////////////////////////////////////////////////////////////

static s7_pointer detree_args(s7_scheme* sc, s7_pointer args)
{
    bool has_tree = false;
    for (auto a = args; !has_tree && a != s7_nil(sc); a = s7_cdr(a))
    {
        auto f = s7_car(a);
        has_tree |= (is_shape(f) &&
                    (get_shape(f)->tree->flags & Tree::FLAG_LOCATION_AGNOSTIC));
    }

    if (has_tree)
    {
        auto new_args = s7_nil(sc);
        for (auto a = args; a != s7_nil(sc); a = s7_cdr(a))
        {
            auto f = s7_car(a);
            new_args = s7_cons(sc, is_shape(f)
                    ? s7_make_real(sc, get_shape(f)->value())
                    : f, new_args);
        }
        return s7_reverse(sc, new_args);
    }
    else
    {
        return args;
    }
}

#define DETREE_FUNC(NAME, STR) \
s7_pointer NAME(s7_scheme* sc, s7_pointer args)             \
{                                                           \
    static s7_pointer op = s7_eval_c_string(sc, "#_" STR);  \
    args = detree_args(sc, args);                           \
    return s7_apply_function(sc, op, args);                 \
}                                                           \

DETREE_FUNC(custom_lt, "<");
DETREE_FUNC(custom_leq, "<=");
DETREE_FUNC(custom_gt, ">");
DETREE_FUNC(custom_geq, ">=");
DETREE_FUNC(custom_equ, "=");
DETREE_FUNC(custom_eq, "eq?");
DETREE_FUNC(custom_equal, "equal?");

////////////////////////////////////////////////////////////////////////////////
// Other helper functions

s7_pointer shape_const_var(s7_scheme* sc, s7_pointer args)
{
    auto obj = s7_car(args);
    CHECK_SHAPE(obj);

    auto& tree = get_shape(obj)->tree;
    if (tree->flags & Tree::FLAG_LOCATION_AGNOSTIC)
    {
        auto out = Tree(Opcode::CONST_VAR, tree);
        return shape_from_tree(sc, out, get_shape(obj)->vars);
    }
    return obj;
}

static s7_pointer shape_lambda(s7_scheme* sc, s7_pointer args)
{
    s7_pointer ms = s7_make_symbol(sc, "make-shape");
    s7_pointer lambda = s7_make_symbol(sc, "lambda");
    return s7_cons(sc, ms, s7_list(sc, 1, s7_cons(sc, lambda, args)));
}

static s7_pointer shape_define(s7_scheme* sc, s7_pointer args)
{
    s7_pointer def = s7_make_symbol(sc, "define");
    s7_pointer ms = s7_make_symbol(sc, "make-shape");
    s7_pointer lambda = s7_make_symbol(sc, "lambda");
    s7_pointer out = s7_list(sc, 3,
        def,
        s7_car(s7_car(args)),
        s7_cons(sc, ms, s7_list(sc, 1,
            s7_cons(sc, lambda, s7_cons(sc, s7_cdr(s7_car(args)),
                                s7_cdr(args))))));
    return out;
}

////////////////////////////////////////////////////////////////////////////////

void init(s7_scheme* sc)
{
    shape_t::tag = s7_new_type_x(sc, "shape",
        shape_print,
        shape_free,
        shape_equal,
        nullptr,  /* gc_mark */
        shape_apply,  /* apply */
        nullptr,  /* set */
        nullptr,  /* length */
        nullptr,  /* copy */
        nullptr,  /* reverse */
        nullptr); /* fill */

    s7_define_function(sc, "make-shape", shape_from_lambda, 1, 0, false,
            "(make-shape func) makes a new shape");
    s7_define_function(sc, "const", shape_const_var, 1, 0, false,
            "(const value) returns a constant number");
    s7_define_function(sc, "shape?", is_shape, 1, 0, false,
            "(shape? s) checks if something is a shape");
    s7_define_macro(sc, "lambda-shape", shape_lambda, 2, 0, true,
            "(lambda-shape (x y z) body) -> (make-shape (lambda (x y z) body))");
    s7_define_macro(sc, "define-shape", shape_define, 2, 0, true,
            "(define-shape (name x y z) body) -> (define name (lambda (x y z) body))");

    install_overload(sc, "+", 0, 0, true, shape_add);
    install_overload(sc, "*", 0, 0, true, shape_mul);
    install_overload(sc, "min", 1, 0, true, shape_min);
    install_overload(sc, "max", 1, 0, true, shape_max);
    install_overload(sc, "-", 1, 0, true, shape_sub);
    install_overload(sc, "/", 1, 0, true, shape_div);
    install_overload(sc, "atan", 1, 1, false, shape_atan);
    install_overload(sc, "expt", 2, 0, false, shape_expt);
    install_overload(sc, "modulo", 2, 0, false, shape_modulo);

    install_overload(sc, "square", 1, 0, false, shape_square);
    install_overload(sc, "sqrt", 1, 0, false, shape_sqrt);
    install_overload(sc, "abs", 1, 0, false, shape_abs);
    install_overload(sc, "sin", 1, 0, false, shape_sin);
    install_overload(sc, "cos", 1, 0, false, shape_cos);
    install_overload(sc, "tan", 1, 0, false, shape_tan);
    install_overload(sc, "asin", 1, 0, false, shape_asin);
    install_overload(sc, "acos", 1, 0, false, shape_acos);
    install_overload(sc, "exp", 1, 0, false, shape_exp);

    install_overload(sc, "<", 0, 0, true, custom_lt);
    install_overload(sc, "<=", 0, 0, true, custom_leq);
    install_overload(sc, ">", 0, 0, true, custom_gt);
    install_overload(sc, ">=", 0, 0, true, custom_geq);
    install_overload(sc, "=", 0, 0, true, custom_equ);
    install_overload(sc, "eq?", 0, 0, true, custom_eq);
    install_overload(sc, "equal?", 0, 0, true, custom_equal);
}

}   // namespace Bind
}   // namespace Kernel
