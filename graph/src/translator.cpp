#include "graph/translator.hpp"

namespace Graph
{

Translator::Translator()
    : target(std::this_thread::get_id())
{
    // Nothing to do here
}

}   // namespace Graph
