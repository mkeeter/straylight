#include "s7/s7.h"

#include "kernel/bind/bind_s7.h"

////////////////////////////////////////////////////////////////////////////////

static int shape_type_tag;

// Define custom types, etc here
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
    (void)args; // TODO: use args
    auto out = new Shape { Kernel::Tree::X() };
    return s7_make_object(sc, shape_type_tag, (void*)out);
}

static s7_pointer shape_is(s7_scheme *sc, s7_pointer args)
{
    return(s7_make_boolean(sc,
        s7_is_object(s7_car(args)) &&
        s7_object_type(s7_car(args)) == shape_type_tag));
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
    s7_define_function(sc, "shape?", shape_is, 1, 0, false,
            "(shape? s) checks if something is a shape");
}
