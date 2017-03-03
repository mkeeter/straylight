#pragma once

#include <QVector3D>

#include "graph/translator.hpp"
#include "kernel/eval/evaluator.hpp"

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
    EscapedPointHandle(Kernel::Tree xyz[3]);
    int tag() const override;

    QVector3D pos;
    Kernel::Evaluator xyz[3];
};

////////////////////////////////////////////////////////////////////////////////


}   // namespace Bridge
}   // namespace App
