#pragma once

#include <glm/vec3.hpp>
#include <memory>

#include "render/drag.hpp"

struct s7_scheme;
struct s7_cell;

namespace App {
namespace Bind {

struct point_handle_t {
    point_handle_t(float x, float y, float z, const std::string& axes,
                   App::Render::Drag* d)
        : pos{x, y, z}, axes(axes), drag(d) {}

    float pos[3];

    /*  'x', 'xy', 'yz', 'xyz', etc */
    std::string axes;

    /*  Drag handle  */
    std::unique_ptr<App::Render::Drag> drag;

    // Type tag for interpreter
    static int tag;
};

/*
 *  Inject app bindings into the given interpreter
 */
void init(s7_scheme* interpreter);

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

}   // namespace Bind
}   // namespace App
