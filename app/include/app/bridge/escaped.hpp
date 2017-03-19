#pragma once

#include <QVector3D>

#include "graph/translator.hpp"
#include "kernel/eval/evaluator.hpp"

namespace App {
namespace Bridge {

////////////////////////////////////////////////////////////////////////////////

class EscapedHandle : public Graph::Escaped
{
public:
    EscapedHandle(const std::map<Kernel::Tree::Id, float>& vars)
        : vars(vars) {};

    virtual int tag() const=0;
    std::map<Kernel::Tree::Id, float> vars;
};

////////////////////////////////////////////////////////////////////////////////

class EscapedPointHandle : public EscapedHandle
{
public:
    EscapedPointHandle(Kernel::Tree xyz[3],
                       const std::map<Kernel::Tree::Id, float>& vars);
    int tag() const override;

    QVector3D pos;
    Kernel::Tree xyz[3];
};

////////////////////////////////////////////////////////////////////////////////

class EscapedShape : public EscapedHandle
{
public:
    EscapedShape(Kernel::Tree tree,
                 const std::map<Kernel::Tree::Id, float>& vars);
    int tag() const override;

    Kernel::Tree tree;
};

////////////////////////////////////////////////////////////////////////////////


}   // namespace Bridge
}   // namespace App
