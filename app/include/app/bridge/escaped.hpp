#pragma once

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

}   // namespace Bridge
}   // namespace App
