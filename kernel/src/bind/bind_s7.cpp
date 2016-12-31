#include "s7/s7.h"

#include "kernel/bind/bind_s7.h"

// Populated in kernel_bind_s7; left uninitialized to make Valgrind warn
// if we ever try to use it at the wrong time.
static int shape_type_tag;

////////////////////////////////////////////////////////////////////////////////

static s7_pointer to_shape(s7_scheme* sc, Kernel::Tree t)
{
    return s7_make_object(sc, shape_type_tag, new Shape { t });
}

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

static bool shape_is(s7_pointer s)
{
    return s7_is_object(s) && s7_object_type(s) == shape_type_tag;
}

static s7_pointer shape_is_(s7_scheme *sc, s7_pointer args)
{
    return s7_make_boolean(sc, shape_is(s7_car(args)));
}

////////////////////////////////////////////////////////////////////////////////

static s7_pointer reduce(s7_scheme* sc, s7_pointer list, const char* name,
                         Kernel::Opcode::Opcode op, float* d)
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
                return s7_wrong_number_of_args_error(sc, name, list);
            }
        }
        case 1:
        {
            auto front = s7_car(list);
            if (shape_is(front))
            {
                return front;
            }
            else if (s7_is_number(front))
            {
                return to_shape(sc, Kernel::Tree(s7_number_to_real(sc, front)));
            }
            else
            {
                return s7_wrong_type_arg_error(sc, name, 0, front,
                        "shape or real");
            }
        }
        default:
        {
            auto front = s7_car(list);
            auto rest = reduce(sc, s7_cdr(list), name, op, d);
            if (!shape_is(rest))
            {
                return rest;
            }

            const auto& rhs = static_cast<Shape*>(s7_object_value(rest))->tree;
            if (s7_is_number(front))
            {
                return to_shape(sc, Kernel::Tree(op,
                        Kernel::Tree(s7_number_to_real(sc, front)), rhs));
            }
            else if (shape_is(front))
            {
                return to_shape(sc, Kernel::Tree(op,
                        static_cast<Shape*>(s7_object_value(front))->tree, rhs));
            }
            else
            {
                return s7_wrong_type_arg_error(sc, name, 0, s7_car(list),
                        "shape or real");
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

static s7_pointer check_result(s7_scheme* sc, s7_pointer out)
{
    if (!shape_is(out))
    {
        return out;
    }
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

#define OVERLOAD_COMMUTATIVE_DEFAULT(NAME, FUNC, OPCODE, DEFAULT)  \
static s7_pointer NAME(s7_scheme* sc, s7_pointer args)          \
{                                                               \
    float d = DEFAULT;                                          \
    return check_result(sc, reduce(sc, args, FUNC, OPCODE, &d));   \
}

#define OVERLOAD_COMMUTATIVE(NAME, FUNC, OPCODE)  \
static s7_pointer NAME(s7_scheme* sc, s7_pointer args)          \
{                                                               \
    return check_result(sc, reduce(sc, args, FUNC, OPCODE, nullptr));   \
}

OVERLOAD_COMMUTATIVE_DEFAULT(shape_add, "+", Kernel::Opcode::ADD, 0);
OVERLOAD_COMMUTATIVE_DEFAULT(shape_mul, "*", Kernel::Opcode::MUL, 1);
OVERLOAD_COMMUTATIVE(shape_min, "min", Kernel::Opcode::MIN);
OVERLOAD_COMMUTATIVE(shape_max, "max", Kernel::Opcode::MAX);

void install_overload(s7_scheme* sc, const char* op,
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
    s7_define_function(sc, "shape?", shape_is_, 1, 0, false,
            "(shape? s) checks if something is a shape");

    install_overload(sc, "+", shape_add);
    install_overload(sc, "*", shape_mul);
    install_overload(sc, "min", shape_min);
    install_overload(sc, "max", shape_max);
}
