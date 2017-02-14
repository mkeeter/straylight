#pragma once

#include <glm/vec3.hpp>

struct s7_scheme;
struct s7_cell;

namespace App {
namespace Bind {

struct point_handle_t {
    float pos[3];

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
const point_handle_t* get_point_handle(s7_cell* obj);

/*
 *  Returns the handle tag
 *  obj must be a valid handle
 */
int get_handle_tag(s7_cell* obj);

}   // namespace Bind
}   // namespace App
