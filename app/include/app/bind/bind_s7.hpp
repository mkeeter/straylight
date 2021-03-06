#pragma once

#include <glm/vec3.hpp>
#include <memory>

#include "app/render/point_drag.hpp"

#include "graph/types/keys.hpp"
#include "kernel/tree/cache.hpp"

struct s7_scheme;
struct s7_cell;

////////////////////////////////////////////////////////////////////////////////

namespace App {
namespace Bind {

////////////////////////////////////////////////////////////////////////////////

struct point_handle_t {
    point_handle_t(Kernel::Tree x, Kernel::Tree y, Kernel::Tree z,
                   const std::map<Kernel::Tree::Id, float>& vars);

    Kernel::Tree xyz[3];
    std::map<Kernel::Tree::Id, float> vars;

    // Type tag for interpreter
    static int tag;
};

/*
 *  Checks if the given object is a point_handle_t
 */
bool is_point_handle(s7_cell* obj);

/*
 *  Extracts a point_handle_t, or nullptr
 */
point_handle_t* get_point_handle(s7_cell* obj);

/*
 *  Returns the handle tag
 *  obj must be a valid handle
 */
int get_handle_tag(s7_cell* obj);

////////////////////////////////////////////////////////////////////////////////

void init(s7_scheme* sc);

}   // namespace Bind
}   // namespace App
