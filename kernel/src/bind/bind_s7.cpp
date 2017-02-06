#include "s7/s7.h"

#include "kernel/bind/bind_s7.h"
#include "kernel/eval/evaluator.hpp"

namespace Kernel {
namespace Bind {

// Populated in bind_s7
int Shape::tag = -1;

////////////////////////////////////////////////////////////////////////////////

/*
 *  Converts a Tree into an allocated Scheme shape
 */
static s7_pointer shape_new(s7_scheme* sc, Kernel::Tree t)
{
    return s7_make_object(sc, Shape::tag, new Shape { t });
}

/*
 *  Shape constructor from an arbitrary function object
 */
static s7_pointer shape_new(s7_scheme* sc, s7_pointer args)
{
    auto obj = s7_call(sc, s7_car(args), s7_list(sc, 3,
            shape_new(sc, Kernel::Tree::affine(1, 0, 0, 0)),
            shape_new(sc, Kernel::Tree::affine(0, 1, 0, 0)),
            shape_new(sc, Kernel::Tree::affine(0, 0, 1, 0))));

    if (s7_is_number(obj))
    {
        return shape_new(sc, Kernel::Tree(s7_number_to_real(sc, obj)));
    }
    else
    {
        return obj;
    }
}

/*
 *  Shape constructor from arbitrary Scheme object
 *
 *  On failure, returns an error message with the given function name
 */
static s7_pointer shape_new(s7_scheme* sc, s7_pointer obj,
                            const char* func_name="shape_new")
{
    if (is_shape(obj))
    {
        return obj;
    }
    else if (s7_is_number(obj))
    {
        return shape_new(sc, Kernel::Tree(s7_number_to_real(sc, obj)));
    }
    else
    {
        return s7_wrong_type_arg_error(sc, func_name, 0, obj,
                "shape or real");
    }
}

#define CHECK_SHAPE(OBJ) {   if (!is_shape(OBJ)) return OBJ; }

////////////////////////////////////////////////////////////////////////////////

static Kernel::Tree to_tree(s7_pointer obj)
{
    return get_shape(obj)->tree;
}

////////////////////////////////////////////////////////////////////////////////

static void shape_free(void* s)
{
    delete static_cast<Shape*>(s);
}

static bool shape_equal(void* a, void* b)
{
    return static_cast<Shape*>(a)->tree ==
           static_cast<Shape*>(b)->tree;
}

static char* shape_print(s7_scheme* sc, void* s)
{
    (void)sc;

    Tree t = static_cast<Shape*>(s)->tree;
    std::stringstream ss;

    if (t.flags() & Tree::FLAG_LOCATION_AGNOSTIC)
    {
        if (t.opcode() == Kernel::Opcode::VAR)
        {
            ss << t.value() << " (variable)";
        }
        else
        {
            ss << t.value() << " (expression)";
        }
    }
    else
    {
        ss << "#<shape at " << s << ">";
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

    auto e = Kernel::Evaluator(to_tree(obj));
    return s7_make_real(sc,
            e.eval(s7_number_to_real(sc, s7_car(args)),
                   s7_number_to_real(sc, s7_cadr(args)),
                   s7_number_to_real(sc, s7_caddr(args))));
}

bool is_shape(s7_pointer s)
{
    return s7_is_object(s) && s7_object_type(s) == Shape::tag;
}

static s7_pointer is_shape(s7_scheme *sc, s7_pointer args)
{
    return s7_make_boolean(sc, is_shape(s7_car(args)));
}

const Shape* get_shape(s7_pointer obj)
{
    return static_cast<Shape*>(s7_object_value_checked(obj, Shape::tag));
}

static Shape* get_mutable_shape(s7_pointer obj)
{
    return static_cast<Shape*>(s7_object_value_checked(obj, Shape::tag));
}

////////////////////////////////////////////////////////////////////////////////

static s7_pointer reduce(s7_scheme* sc, s7_pointer list, const char* func_name,
                         Kernel::Opcode::Opcode op, const float* d)
{
    switch (s7_list_length(sc, list))
    {
        case 0:
        {
            // On an empty list, return the default value or an error
            if (d != nullptr)
            {
                return shape_new(sc, Kernel::Tree(*d));
            }
            else
            {
                return s7_wrong_number_of_args_error(sc, func_name, list);
            }
        }
        case 1:
        {
            return shape_new(sc, s7_car(list), func_name);
        }
        default:
        {
            auto front = shape_new(sc, s7_car(list), func_name);
            CHECK_SHAPE(front);

            auto rest = reduce(sc, s7_cdr(list), func_name, op, d);
            CHECK_SHAPE(rest);

            return shape_new(sc, Kernel::Tree(op, to_tree(front), to_tree(rest)));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

static s7_pointer result_to_const(s7_scheme* sc, s7_pointer out)
{
    CHECK_SHAPE(out);

    auto& tree = get_mutable_shape(out)->tree;

    // If the result is a constant (indicating that there was no Shape
    // involved in the reduction), then convert back to an ordinary
    // Scheme real to keep addition working as usual.
    if (tree.opcode() == Kernel::Opcode::CONST)
    {
        return s7_make_real(sc, tree.value());
    }
    // If this is a location-agnostic expression but not directly a VAR,
    // then make sure that it has the correct value
    else if ((tree.flags() & Tree::FLAG_LOCATION_AGNOSTIC) &&
             (tree.opcode() != Kernel::Opcode::VAR))
    {
        tree.checkValue();
    }
    return out;
}

#define OVERLOAD_COMMUTATIVE_DEFAULT(NAME, FUNC, OPCODE, DEFAULT)           \
static s7_pointer NAME(s7_scheme* sc, s7_pointer args)                      \
{                                                                           \
    float d = DEFAULT;                                                      \
    return result_to_const(sc, reduce(sc, args, FUNC, OPCODE, &d));         \
}

#define OVERLOAD_COMMUTATIVE(NAME, FUNC, OPCODE)                            \
static s7_pointer NAME(s7_scheme* sc, s7_pointer args)                      \
{                                                                           \
    return result_to_const(sc, reduce(sc, args, FUNC, OPCODE, nullptr));    \
}

OVERLOAD_COMMUTATIVE_DEFAULT(shape_add, "+", Kernel::Opcode::ADD, 0);
OVERLOAD_COMMUTATIVE_DEFAULT(shape_mul, "*", Kernel::Opcode::MUL, 1);
OVERLOAD_COMMUTATIVE(shape_min, "min", Kernel::Opcode::MIN);
OVERLOAD_COMMUTATIVE(shape_max, "max", Kernel::Opcode::MAX);

static s7_pointer shape_sub(s7_scheme* sc, s7_pointer args)
{
    switch (s7_list_length(sc, args))
    {
        case 0:
        {
            return s7_wrong_number_of_args_error(sc, "-: too few arguments: ~A", args);
        }
        case 1:
        {
            auto s = shape_new(sc, s7_car(args), "-");
            CHECK_SHAPE(s);
            return result_to_const(sc, shape_new(sc,
                        Kernel::Tree(Kernel::Opcode::NEG, to_tree(s))));
        }
        default:
        {
            auto lhs = shape_new(sc, s7_car(args), "-");
            CHECK_SHAPE(lhs);
            const float d = 0;
            auto rhs = reduce(sc, s7_cdr(args), "-", Kernel::Opcode::ADD, &d);
            CHECK_SHAPE(rhs);
            return result_to_const(sc, shape_new(sc,
                        Kernel::Tree(Kernel::Opcode::SUB,
                            to_tree(lhs), to_tree(rhs))));
        }
    }
}

static s7_pointer shape_div(s7_scheme* sc, s7_pointer args)
{
    switch (s7_list_length(sc, args))
    {
        case 0:
        {
            return s7_wrong_number_of_args_error(sc, "/: too few arguments: ~A", args);
        }
        case 1:
        {
            auto s = shape_new(sc, s7_car(args), "/");
            CHECK_SHAPE(s);
            return result_to_const(sc, shape_new(sc,
                        Kernel::Tree(Kernel::Opcode::DIV, 1.0f, to_tree(s))));
        }
        default:
        {
            auto lhs = shape_new(sc, s7_car(args), "/");
            CHECK_SHAPE(lhs);
            const float d = 1;
            auto rhs = reduce(sc, s7_cdr(args), "/", Kernel::Opcode::MUL, &d);
            CHECK_SHAPE(rhs);
            return result_to_const(sc, shape_new(sc,
                        Kernel::Tree(Kernel::Opcode::DIV,
                            to_tree(lhs), to_tree(rhs))));
        }
    }
}

static s7_pointer shape_atan(s7_scheme* sc, s7_pointer args)
{
    switch (s7_list_length(sc, args))
    {
        case 1:
        {
            auto s = shape_new(sc, s7_car(args), "atan");
            CHECK_SHAPE(s);
            return result_to_const(sc, shape_new(sc,
                        Kernel::Tree(Kernel::Opcode::ATAN, to_tree(s))));
        }
        case 2:
        {
            auto a = shape_new(sc, s7_car(args), "atan");
            CHECK_SHAPE(a);
            auto b = shape_new(sc, s7_cadr(args), "atan");
            CHECK_SHAPE(b);
            return result_to_const(sc, shape_new(sc,
                        Kernel::Tree(Kernel::Opcode::ATAN2, to_tree(a), to_tree(b))));
        }
        default:    return s7_wrong_number_of_args_error(sc, "atan: wrong number of args: ~A", args);
    }
}

static s7_pointer shape_expt(s7_scheme* sc, s7_pointer args)
{
    if (s7_list_length(sc, args) == 2)
    {
        auto a = s7_car(args);
        auto b = s7_cadr(args);

        if (s7_is_number(a) && s7_is_number(b))
        {
            return s7_make_real(sc, pow(s7_number_to_real(sc, a),
                                        s7_number_to_real(sc, b)));
        }

        auto lhs = shape_new(sc, s7_car(args), "expt");
        CHECK_SHAPE(a);

        if (s7_is_integer(b))
        {
            return shape_new(sc, Kernel::Tree(Kernel::Opcode::POW,
                to_tree(lhs), Kernel::Tree(s7_integer(b))));
        }
        else if (s7_is_ratio(b) && s7_numerator(b) == 1)
        {
            return shape_new(sc, Kernel::Tree(Kernel::Opcode::NTH_ROOT,
                to_tree(lhs), Kernel::Tree(s7_denominator(b))));
        }
        else
        {
            return s7_wrong_type_arg_error(sc, "expt", 2, b,
                    "integer or 1/integer");
        }
    }
    else
    {
        return s7_wrong_number_of_args_error(sc,
                "expt: wrong number of args: ~A", args);
    }
}

static s7_pointer shape_modulo(s7_scheme* sc, s7_pointer args)
{
    if (s7_list_length(sc, args) != 2)
    {
        return s7_wrong_number_of_args_error(sc,
                "modulo: wrong number of args: ~A", args);
    }

    auto lhs = shape_new(sc, s7_car(args), "modulo");
    CHECK_SHAPE(lhs);
    auto rhs = shape_new(sc, s7_cadr(args), "modulo");
    CHECK_SHAPE(rhs);

    return result_to_const(sc, shape_new(sc, Kernel::Tree(Kernel::Opcode::MOD,
        to_tree(lhs), to_tree(rhs))));
}

#define OVERLOAD_UNARY(NAME, FUNC, OPCODE) \
static s7_pointer NAME(s7_scheme* sc, s7_pointer args) \
{                                                       \
    if (s7_list_length(sc, args) != 1)                  \
    {                                                   \
        return s7_wrong_number_of_args_error(sc,        \
                FUNC ": wrong number of args: ~A", args);   \
    }   \
    auto lhs = shape_new(sc, s7_car(args), FUNC);    \
    CHECK_SHAPE(lhs);   \
    return result_to_const(sc,  \
            shape_new(sc, Kernel::Tree(OPCODE, to_tree(lhs)))); \
}
OVERLOAD_UNARY(shape_square, "square", Kernel::Opcode::SQUARE);
OVERLOAD_UNARY(shape_sqrt, "sqrt", Kernel::Opcode::SQRT);
OVERLOAD_UNARY(shape_abs, "abs", Kernel::Opcode::ABS);
OVERLOAD_UNARY(shape_sin, "sin", Kernel::Opcode::SIN);
OVERLOAD_UNARY(shape_cos, "cos", Kernel::Opcode::COS);
OVERLOAD_UNARY(shape_tan, "tan", Kernel::Opcode::TAN);
OVERLOAD_UNARY(shape_asin, "asin", Kernel::Opcode::ASIN);
OVERLOAD_UNARY(shape_acos, "acos", Kernel::Opcode::ACOS);
OVERLOAD_UNARY(shape_exp, "exp", Kernel::Opcode::EXP);

////////////////////////////////////////////////////////////////////////////////

static void install_overload(s7_scheme* sc, const char* op,
                      s7_pointer (*f)(s7_scheme*, s7_pointer))
{
    s7_define_function(sc, op, f, 0, 0, true,
                       s7_help(sc, s7_name_to_value(sc, op)));
}

////////////////////////////////////////////////////////////////////////////////

s7_pointer reader(s7_scheme* sc, s7_pointer args)
{
    args = s7_car(args);
    if (s7_list_length(sc, args) == 1 && s7_is_number(s7_car(args)))
    {
        auto env_tree_map = s7_name_to_value(sc, "*env-tree-map*");
        auto env = s7_name_to_value(sc, "*env*");
        auto cell_ref = s7_hash_table_ref(sc, env_tree_map, env);

        const auto v = s7_number_to_real(sc, s7_car(args));
        if (cell_ref == s7_f(sc))
        {
            cell_ref = shape_new(sc, Kernel::Tree::var(v));
            s7_hash_table_set(sc, env_tree_map, env, cell_ref);
        }
        else
        {
            to_tree(cell_ref).setValue(v);
        }

        return s7_list(sc, 1, cell_ref);
    }
    return args;
}

////////////////////////////////////////////////////////////////////////////////

void init(s7_scheme* sc)
{
    Shape::tag = s7_new_type_x(sc, "shape",
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

    s7_define_function(sc, "make-shape", shape_new, 1, 0, false,
            "(make-shape func) makes a new shape");
    s7_define_function(sc, "shape?", is_shape, 1, 0, false,
            "(shape? s) checks if something is a shape");

    install_overload(sc, "+", shape_add);
    install_overload(sc, "*", shape_mul);
    install_overload(sc, "min", shape_min);
    install_overload(sc, "max", shape_max);
    install_overload(sc, "-", shape_sub);
    install_overload(sc, "/", shape_div);
    install_overload(sc, "atan", shape_atan);
    install_overload(sc, "expt", shape_expt);
    install_overload(sc, "modulo", shape_modulo);

    install_overload(sc, "square", shape_square);
    install_overload(sc, "sqrt", shape_sqrt);
    install_overload(sc, "abs", shape_abs);
    install_overload(sc, "sin", shape_sin);
    install_overload(sc, "cos", shape_cos);
    install_overload(sc, "tan", shape_tan);
    install_overload(sc, "asin", shape_asin);
    install_overload(sc, "acos", shape_acos);
    install_overload(sc, "exp", shape_exp);

    s7_define_function(sc, "*cell-reader*", reader, 1, 0, 0,
        "Reads a list of s-exprs, recording solo floats in *env-tree-map*");

    s7_define_constant(sc, "*env-tree-map*", s7_make_hash_table(sc, 128));

}

}   // namespace Bind
}   // namespace Kernel
