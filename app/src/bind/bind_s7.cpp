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

////////////////////////////////////////////////////////////////////////////////

point_handle_t::point_handle_t(Kernel::Tree x, Kernel::Tree y, Kernel::Tree z,
                               const std::map<Kernel::Tree::Id, float>& vars)
    : xyz{x, y, z}, vars(vars)
{
    // Nothing to do here
}

////////////////////////////////////////////////////////////////////////////////

static char* point_handle_print(s7_scheme* sc, void* s)
{
    (void)sc;
    std::string desc = "#<point-handle";

    auto p = static_cast<point_handle_t*>(s);

    std::string axes;
    for (unsigned i=0; i < 3; ++i)
    {
        if (p->xyz[i]->op != Kernel::Opcode::CONST)
        {
            axes += "xyz"[i];
        }
    }

    if (axes.length())
    {
        desc += "-" + axes;
    }
    desc += ">";

    auto out = static_cast<char*>(calloc(desc.length() + 1, sizeof(char)));
    memcpy(out, &desc[0], desc.length());
    return out;
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
                if (!(Kernel::Bind::get_shape(args[i])->tree->flags &
                      Kernel::Tree::FLAG_LOCATION_AGNOSTIC))
                {
                    return s7_wrong_type_arg_error(sc, "point_handle_new", i,
                            args[i], "location-agnostic tree");
                }
            }
        }
    }

    // Convert from objects into bound shapes
    auto x = Kernel::Bind::shape_from_obj(sc, _x);
    auto y = Kernel::Bind::shape_from_obj(sc, _y);
    auto z = Kernel::Bind::shape_from_obj(sc, _z);

    // Assert that the bindings were successful
    assert(Kernel::Bind::is_shape(x) &&
           Kernel::Bind::is_shape(x) &&
           Kernel::Bind::is_shape(x));

    // Take the union of all the variables
    std::map<Kernel::Tree::Id, float> vars;
    for (auto& a : {x, y, z})
    {
        auto s = Kernel::Bind::get_shape(a);
        vars.insert(s->vars.begin(), s->vars.end());
    }

    return s7_make_object(sc, point_handle_t::tag,
            new point_handle_t(
                Kernel::Bind::get_shape(x)->tree,
                Kernel::Bind::get_shape(y)->tree,
                Kernel::Bind::get_shape(z)->tree, vars));
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
    auto graph = App::Bridge::GraphModel::instance();
    const auto& cell = *static_cast<Graph::CellKey*>(
            s7_c_pointer(s7_cadr(args)));

    // Previous value is passed into reader for exactly this use case
    s7_pointer prev_value = s7_caddr(args);
    const bool was_shape_var =
        Kernel::Bind::get_shape(prev_value) &&
        Kernel::Bind::get_shape(prev_value)->tree->op == Kernel::Opcode::VAR;

    // If this is a single-clause expression that is a number, wrap it
    // into a tree variable so that we can manipulate it later on
    if (s7_list_length(sc, begin) == 1 && s7_is_number(s7_car(begin)))
    {
        // Extract the numerical value for this variable
        const auto v = s7_number_to_real(sc, s7_car(begin));

        // If this variable is already in use, then update its value and
        // return a Shape with the new variable value (which will be
        // marked as not equal because of the vars map in each shape)
        if (was_shape_var)
        {
            // Build a variable with the same Tree but a different value
            auto shape = Kernel::Bind::get_shape(prev_value);
            auto var = Kernel::Bind::shape_from_tree(sc,
                    shape->tree, {{shape->tree.id(), v}});
            return s7_list(sc, 1, var);
        }
        // Otherwise, make a new variable and save it in the graph model
        else
        {
            auto tree = Kernel::Tree::var();
            graph->defineVar(cell, tree.id());
            return s7_list(sc, 1, Kernel::Bind::shape_from_tree(
                        sc, tree, {{tree.id(), v}}));
        }
    }
    // If the previous value was a variable, but this is not, tell the
    // graph to forget about the previous value
    else if (was_shape_var)
    {
        graph->forgetVar(Kernel::Bind::get_shape(prev_value)->tree.id());
    }
    return begin;
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

    s7_define_function(sc, "*cell-reader*", reader, 3, 0, 0,
        "Reads a list of s-exprs, doing special things to floats");
}

}   // namespace Bind
}   // namespace App
