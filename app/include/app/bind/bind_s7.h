#pragma once

#include <glm/vec3.hpp>
#include <memory>

#include "app/render/drag.hpp"

#include "graph/types/keys.hpp"
#include "graph/types/tag.hpp"
#include "kernel/tree/tag.hpp"
#include "kernel/tree/cache.hpp"

struct s7_scheme;
struct s7_cell;

////////////////////////////////////////////////////////////////////////////////

namespace App {
namespace Bind {

////////////////////////////////////////////////////////////////////////////////

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

/*
 *  Here, we define a pair of tags used to connect variables (in the math tree
 *  with where they are defined (in the computation graph).
 *
 *  CellKeyTag is stored in the math kernel's Cache and points to a particular
 *  CellKey.
 *
 *  IdTag is stored in the graph's Root object and points to a particular
 *  Id in the cache.
 */
class CellKeyTag : public Kernel::Tag
{
public:
    CellKeyTag(const Graph::CellKey& k) : key(k) {}
    Tag* clone() const override { return new CellKeyTag(key); }

    const Graph::CellKey key;
};

class IdTag : public Graph::Tag
{
public:
    IdTag(const Kernel::Cache::VarId& i) : id(i) {}
    const Kernel::Cache::VarId id;
};

////////////////////////////////////////////////////////////////////////////////

void init(s7_scheme* sc);

}   // namespace Bind
}   // namespace App
