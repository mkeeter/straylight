#pragma once

#include "render/handle.hpp"
#include "bind/bind_s7.h"

namespace App {
namespace Render {

class PointHandle : public Handle
{
public:
    PointHandle(const App::Bind::point_handle_t* p);

    void draw(const QMatrix4x4& m, bool selected) override;
    void updateFrom(Graph::ValuePtr p) override;
};

}   // namespace Render
}   // namespace App
