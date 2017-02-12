#pragma once

#include <QMatrix4x4>

#include "graph/ptr.hpp"

namespace App {
namespace Render {

class Handle
{
public:
    virtual ~Handle() { /* Nothing to do here */ }
    virtual void draw(const QMatrix4x4& m, bool selected)=0;
    virtual void updateFrom(Graph::ValuePtr p)=0;
};

}   // namespace Render
}   // namespace App
