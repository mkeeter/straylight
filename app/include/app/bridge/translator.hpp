#pragma once

#include "graph/translator.hpp"

namespace App {
namespace Bridge {

class Translator : public Graph::Translator
{
public:
    Translator();
    Graph::Escaped* operator()(Graph::ValuePtr ptr) override;
};

}   // namespace Bridge
}   // namespace App
