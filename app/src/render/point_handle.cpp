#include "render/point_handle.hpp"

namespace App {
namespace Render {

PointHandle::PointHandle(const App::Bind::point_handle_t* p)
{
    (void)p;
}

void PointHandle::draw(const QMatrix4x4& m, bool selected)
{
    (void)m;
    (void)selected;
}

void PointHandle::updateFrom(Graph::ValuePtr ptr)
{
    auto p = App::Bind::get_point_handle(ptr);
    (void)p;
}


}   // namespace Render
}   // namespace App
