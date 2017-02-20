#include <sstream>

#include "s7/s7.h"

#include "app/bridge/bridge.hpp"
#include "app/bind/bind_s7.hpp"

#include "kernel/bind/bind_s7.h"
#include "kernel/tree/tree.hpp"
#include "graph/interpreter.hpp"

namespace App {
namespace Bind {

// s7 type tag for point_handle_t
int point_handle_t::tag = -1;

static char* point_handle_print(s7_scheme* sc, void* s)
{
    (void)sc;
    std::string desc = "point-handle";

    auto p = static_cast<point_handle_t*>(s);
    if (p->axes.length())
    {
        desc += "_" + p->axes;
    }

    return Graph::Interpreter::print(desc, s);
}

static void point_handle_free(void* p)
{
    delete static_cast<point_handle_t*>(p);
}

static s7_pointer point_handle_new(s7_scheme* sc, s7_pointer args)
{
    auto _x = s7_car(args);
    auto _y = s7_cadr(args);
    auto _z = s7_caddr(args);

    std::string axes = "";

    {   // Check arguments: they should be location-agnostic trees or numbers
        s7_pointer args[3] = {_x, _y, _z};
        for (int i=0; i < 3; ++i)
        {
            if (!s7_is_number(args[i]) && !Kernel::Bind::is_shape(args[i]))
            {
                return s7_wrong_type_arg_error(sc, "point_handle_new", i,
                        args[i], "number or tree");
            }
            else if (Kernel::Bind::is_shape(args[i]))
            {
                if (!(Kernel::Bind::get_shape(args[i])->tree.flags() &
                      Kernel::Tree::FLAG_LOCATION_AGNOSTIC))
                {
                    return s7_wrong_type_arg_error(sc, "point_handle_new", i,
                            args[i], "location-agnostic tree");
                }
                axes += "xyz"[i];
            }
        }
    }

    auto to_tree = [=](s7_pointer p){
        return s7_is_number(p) ? Kernel::Tree(s7_number_to_real(sc, p))
                               : Kernel::Bind::get_shape(p)->tree;
    };

    auto x = to_tree(_x);
    auto y = to_tree(_y);
    auto z = to_tree(_z);

    return s7_make_object(sc, point_handle_t::tag,
            new point_handle_t(
                x.value(), y.value(), z.value(), axes,
                new App::Render::Drag(x, y, z)));
}

bool is_point_handle(s7_pointer s)
{
    return s7_is_object(s) && s7_object_type(s) == point_handle_t::tag;
}

point_handle_t* get_point_handle(s7_cell* obj)
{
    return static_cast<point_handle_t*>(
            s7_object_value_checked(obj, point_handle_t::tag));
}

int get_handle_tag(s7_cell* obj)
{
    assert(s7_is_object(obj));
    return s7_object_type(obj);
}

////////////////////////////////////////////////////////////////////////////////

s7_pointer reader(s7_scheme* sc, s7_pointer args)
{
    auto begin = s7_car(args);

    if (s7_list_length(sc, begin) == 1 && s7_is_number(s7_car(begin)))
    {
        auto& root = *static_cast<Graph::Root*>(s7_c_pointer(s7_cadr(args)));
        auto cache = Kernel::Cache::instance();
        auto& cell = *static_cast<Graph::CellKey*>(s7_c_pointer(s7_caddr(args)));

        const auto v = s7_number_to_real(sc, s7_car(begin));

        // If this variable is already in use, then update its value and
        // return a Shape with the changed flag set if the value changed.
        if (auto t = root.tag(cell))
        {
            assert(dynamic_cast<IdTag*>(t));

            auto id = static_cast<IdTag*>(t)->id;
            const bool changed = cache->value(id) != v;
            cache->setValue(id, v);

            auto var = Kernel::Tree::var(id);
            return s7_list(sc, 1, Kernel::Bind::shape_new_(sc, var, changed));
        }
        // Otherwise, make a new variable and cross-link it into the cache
        // and the graph root.
        else
        {
            auto var = Kernel::Tree::var(v);
            var.setTag(new CellKeyTag(cell));
            root.setTag(cell, new IdTag(var.var()));
            return s7_list(sc, 1, Kernel::Bind::shape_new(sc, var));
        }
    }
    else
    {
        return begin;
    }
}

////////////////////////////////////////////////////////////////////////////////

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

    s7_define_function(sc, "*cell-reader*", reader, 2, 0, 0,
        "Reads a list of s-exprs, doing special things to floats");
}

}   // namespace Bind
}   // namespace App
