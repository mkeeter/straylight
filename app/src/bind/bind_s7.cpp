#include <sstream>

#include "s7/s7.h"

#include "bind/bind_s7.h"
#include "graph/interpreter.hpp"

namespace App {
namespace Bind {

// s7 type tag for point_handle_t
int point_handle_t::tag = -1;

static char* point_handle_print(s7_scheme* sc, void* s)
{
    (void)sc;
    return Graph::Interpreter::print("point-handle", s);
}

static void point_handle_free(void* p)
{
    delete static_cast<point_handle_t*>(p);
}

static s7_pointer point_handle_new(s7_scheme* sc, s7_pointer args)
{
    float x = s7_number_to_real(sc, s7_car(args));
    float y = s7_number_to_real(sc, s7_cadr(args));
    float z = s7_number_to_real(sc, s7_caddr(args));
    return s7_make_object(sc, point_handle_t::tag,
            new point_handle_t { {x,y,z} });
}

bool is_point_handle(s7_pointer s)
{
    return s7_is_object(s) && s7_object_type(s) == point_handle_t::tag;
}

const point_handle_t* get_point_handle(s7_cell* obj)
{
    return static_cast<point_handle_t*>(
            s7_object_value_checked(obj, point_handle_t::tag));
}

int get_handle_tag(s7_cell* obj)
{
    assert(s7_is_object(obj));
    return s7_object_type(obj);
}

void init(s7_scheme* sc)
{
    point_handle_t::tag = s7_new_type_x(sc, "point-handle",
        point_handle_print,
        point_handle_free,
        nullptr,  /* equal */
        nullptr,  /* gc_mark */
        nullptr,  /* apply */
        nullptr,  /* set */
        nullptr,  /* length */
        nullptr,  /* copy */
        nullptr,  /* reverse */
        nullptr); /* fill */

    s7_define_function(sc, "ui-point", point_handle_new, 3, 0, false,
            "(ui-point x y z) makes a point handle in the 3D viewport");
}

}   // namespace Bind
}   // namespace App
