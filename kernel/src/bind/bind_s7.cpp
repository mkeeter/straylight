#include "s7/s7.h"

#include "kernel/bind/bind_s7.h"

// Populated in kernel_bind_s7; left uninitialized to make Valgrind warn
// if we ever try to use it at the wrong time.
static int shape_type_tag;

// Forward declaration
static bool is_shape(s7_pointer s);

////////////////////////////////////////////////////////////////////////////////

/*
 *  Converts a Tree into an allocated Scheme shape
 */
static s7_pointer to_shape(s7_scheme* sc, Kernel::Tree t)
{
    return s7_make_object(sc, shape_type_tag, new Shape { t });
}

static Kernel::Tree to_tree(s7_pointer obj)
{
    assert(is_shape(obj));
    return static_cast<Shape*>(s7_object_value(obj))->tree;
}

/*
 *  Tries to convert a Scheme object into a shape
 *
 *  On failure, returns an error message with the given function name
 */
s7_pointer ensure_shape(s7_scheme* sc, s7_pointer obj,
                        const char* func_name="ensure_shape")
{
    if (is_shape(obj))
    {
        return obj;
    }
    else if (s7_is_number(obj))
    {
        return to_shape(sc, Kernel::Tree(s7_number_to_real(sc, obj)));
    }
    else
    {
        return s7_wrong_type_arg_error(sc, func_name, 0, obj,
                "shape or real");
    }
}
#define CHECK_SHAPE(OBJ) {   if (!is_shape(OBJ)) return OBJ; }

////////////////////////////////////////////////////////////////////////////////

static void shape_free(void* s)
{
    if (s)
    {
        delete static_cast<Shape*>(s);
    }
}

static char* shape_print(s7_scheme* sc, void* s)
{
    (void)sc;

    std::stringstream ss;
    ss << "#<shape at " << s << ">";
    auto str = ss.str();

    auto out = static_cast<char*>(calloc(str.length() + 1, sizeof(char)));
    memcpy(out, &str[0], str.length());
    return out;
}

static s7_pointer shape_new(s7_scheme* sc, s7_pointer args)
{
    (void)args; // TODO: trace args[0] to get Tree
    return to_shape(sc, Kernel::Tree::X());
}

static bool is_shape(s7_pointer s)
{
    return s7_is_object(s) && s7_object_type(s) == shape_type_tag;
}

static s7_pointer is_shape_(s7_scheme *sc, s7_pointer args)
{
    return s7_make_boolean(sc, is_shape(s7_car(args)));
}

////////////////////////////////////////////////////////////////////////////////

static s7_pointer reduce(s7_scheme* sc, s7_pointer list, const char* func_name,
                         Kernel::Opcode::Opcode op, const float* d)
{
    switch (s7_list_length(sc, list))
    {
        case 0:
        {
            if (d != nullptr)
            {
                return to_shape(sc, Kernel::Tree(*d));
            }
            else
            {
                return s7_wrong_number_of_args_error(sc, func_name, list);
            }
        }
        case 1:
        {
            return ensure_shape(sc, s7_car(list), func_name);
        }
        default:
        {
            auto front = ensure_shape(sc, s7_car(list), func_name);
            CHECK_SHAPE(front);

            auto rest = reduce(sc, s7_cdr(list), func_name, op, d);
            CHECK_SHAPE(rest);

            return to_shape(sc, Kernel::Tree(op, to_tree(front), to_tree(rest)));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

static s7_pointer result_to_const(s7_scheme* sc, s7_pointer out)
{
    CHECK_SHAPE(out);

    const auto& tree = static_cast<Shape*>(s7_object_value(out))->tree;

    // If the result is a constant (indicating that there was no Shape
    // involved in the reduction), then convert back to an ordinary
    // Scheme real to keep addition working as usual.
    if (tree.opcode() == Kernel::Opcode::CONST)
    {
        return s7_make_real(sc, tree.value());
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
            return s7_wrong_number_of_args_error(sc, "-", args);
        }
        case 1:
        {
            auto s = ensure_shape(sc, s7_car(args), "-");
            CHECK_SHAPE(s);
            return result_to_const(sc, to_shape(sc,
                        Kernel::Tree(Kernel::Opcode::NEG, to_tree(s))));
        }
        default:
        {
            auto lhs = ensure_shape(sc, s7_car(args), "-");
            CHECK_SHAPE(lhs);
            const float d = 0;
            auto rhs = reduce(sc, s7_cdr(args), "-", Kernel::Opcode::ADD, &d);
            CHECK_SHAPE(rhs);
            return result_to_const(sc, to_shape(sc,
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
            return s7_wrong_number_of_args_error(sc, "/", args);
        }
        case 1:
        {
            auto s = ensure_shape(sc, s7_car(args), "/");
            CHECK_SHAPE(s);
            return result_to_const(sc, to_shape(sc,
                        Kernel::Tree(Kernel::Opcode::DIV, 1.0f, to_tree(s))));
        }
        default:
        {
            auto lhs = ensure_shape(sc, s7_car(args), "/");
            CHECK_SHAPE(lhs);
            const float d = 1;
            auto rhs = reduce(sc, s7_cdr(args), "/", Kernel::Opcode::MUL, &d);
            CHECK_SHAPE(rhs);
            return result_to_const(sc, to_shape(sc,
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
            auto s = ensure_shape(sc, s7_car(args), "atan");
            CHECK_SHAPE(s);
            return result_to_const(sc, to_shape(sc,
                        Kernel::Tree(Kernel::Opcode::ATAN, to_tree(s))));
        }
        case 2:
        {
            auto a = ensure_shape(sc, s7_car(args), "atan");
            CHECK_SHAPE(a);
            auto b = ensure_shape(sc, s7_cadr(args), "atan");
            CHECK_SHAPE(b);
            return result_to_const(sc, to_shape(sc,
                        Kernel::Tree(Kernel::Opcode::ATAN2, to_tree(a), to_tree(b))));
        }
        default:    return s7_wrong_number_of_args_error(sc, "atan", args);
    }
}

////////////////////////////////////////////////////////////////////////////////

static void install_overload(s7_scheme* sc, const char* op,
                      s7_pointer (*f)(s7_scheme*, s7_pointer))
{
    s7_define_function(sc, op, f, 0, 0, true,
                       s7_help(sc, s7_name_to_value(sc, op)));
}

////////////////////////////////////////////////////////////////////////////////

void kernel_bind_s7(s7_scheme* sc)
{
    shape_type_tag = s7_new_type_x(sc, "shape",
        shape_print,
        shape_free,
        nullptr,  /* equal */
        nullptr,  /* gc_mark */
        nullptr,  /* apply */
        nullptr,  /* set */
        nullptr,  /* length */
        nullptr,  /* copy */
        nullptr,  /* reverse */
        nullptr); /* fill */

    s7_define_function(sc, "make-shape", shape_new, 1, 0, false,
            "(make-shape func) makes a new shape");
    s7_define_function(sc, "shape?", is_shape_, 1, 0, false,
            "(shape? s) checks if something is a shape");

    install_overload(sc, "+", shape_add);
    install_overload(sc, "*", shape_mul);
    install_overload(sc, "min", shape_min);
    install_overload(sc, "max", shape_max);
    install_overload(sc, "-", shape_sub);
    install_overload(sc, "/", shape_div);
    install_overload(sc, "atan", shape_atan);
}
