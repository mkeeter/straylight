#include <sstream>

#include "s7/s7.h"

#include "bind/bind_s7.h"
#include "graph/interpreter.hpp"

namespace App {
namespace Bind {

// s7 type tag for PointHandle
int PointHandle::tag = -1;

void is_point_handle(s7_cell* obj);
const PointHandle* get_point_handle(s7_cell* obj);

static char* point_handle_print(s7_scheme* sc, void* s)
{
    (void)sc;
    return Graph::Interpreter::print("point-handle", s);
}

static void point_handle_free(void* p)
{
    delete static_cast<PointHandle*>(p);
}

static s7_pointer point_handle_new(s7_scheme* sc, s7_pointer args)
{
    (void)args;
    return s7_make_object(sc, PointHandle::tag, new PointHandle { {0,0,0}, 1 });
}

void init(s7_scheme* sc)
{
    PointHandle::tag = s7_new_type_x(sc, "point-handle",
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
