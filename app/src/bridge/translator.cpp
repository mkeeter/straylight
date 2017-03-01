#include "app/bridge/translator.hpp"
#include "app/render/renderer.hpp"
#include "app/render/point_handle.hpp"

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
        return new App::Render::Renderer(s->tree);
    }
    else if (App::Bind::is_point_handle(ptr))
    {
        auto p = App::Bind::get_point_handle(ptr);
        return new App::Render::PointHandle(p->drag);
    }

    return nullptr;
}

}   // namespace Bridge
}   // namespace App