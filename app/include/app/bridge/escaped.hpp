#pragma once

#include "graph/translator.hpp"
#include "kernel/eval/evaluator.hpp"
#include "app/render/point_drag.hpp"

namespace App {
namespace Bridge {

class EscapedShape : public Graph::Escaped
{
public:
    EscapedShape(Kernel::Tree tree);
    Kernel::Evaluator* eval;
};

////////////////////////////////////////////////////////////////////////////////

class EscapedHandle : public Graph::Escaped
{
public:
    EscapedHandle() {};

    virtual int tag() const=0;
};

class EscapedPointHandle : public EscapedHandle
{
public:
    EscapedPointHandle(std::unique_ptr<App::Render::PointDrag>& d);
    int tag() const override;

    QVector3D pos;
    std::unique_ptr<App::Render::PointDrag> drag;
};

////////////////////////////////////////////////////////////////////////////////


}   // namespace Bridge
}   // namespace App
