#pragma once

#include <glm/vec3.hpp>

struct s7_scheme;
struct s7_cell;

namespace App {
namespace Bind {

struct PointHandle {
    glm::vec3 pos;
    float r;

    // Type tag for interpreter
    static int tag;
};

/*
 *  Inject app bindings into the given interpreter
 */
void init(s7_scheme* interpreter);

extern "C" {
    void is_point_handle(s7_cell* obj);
    const PointHandle* get_point_handle(s7_cell* obj);
};

}   // namespace Bind
}   // namespace App
