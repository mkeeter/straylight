#include "app/bridge/escaped.hpp"

namespace App {
namespace Bridge {

EscapedShape::EscapedShape(Kernel::Tree tree)
    : eval(new Kernel::Evaluator(tree))
{
    // Nothing to do here
}

}   // namespace Bridge
}   // namespace App
