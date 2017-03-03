#include "app/bridge/translator.hpp"
#include "app/bridge/escaped.hpp"

#include "app/bind/bind_s7.hpp"
#include "kernel/bind/bind_s7.h"

namespace App {
namespace Bridge {

Translator::Translator()
    : Graph::Translator()
{
    // Nothing to do here
}

Graph::Escaped* Translator::operator()(Graph::ValuePtr ptr)
{
    if (Kernel::Bind::is_shape(ptr))
    {
        auto s = Kernel::Bind::get_shape(ptr);
        if (!(s->tree.flags() & Kernel::Tree::FLAG_LOCATION_AGNOSTIC))
        {
            return new App::Bridge::EscapedShape(s->tree);
        }
    }
    else if (App::Bind::is_point_handle(ptr))
    {
        auto p = App::Bind::get_point_handle(ptr);
        return new App::Bridge::EscapedPointHandle(
                {p->pos[0], p->pos[1], p->pos[2]}, p->drag);
    }

    return nullptr;
}

}   // namespace Bridge
}   // namespace App
