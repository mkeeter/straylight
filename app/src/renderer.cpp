#include "renderer.hpp"

Renderer::Renderer(Kernel::Tree t)
{
    printf("Making new renderer\n");
    for (int i=0; i < 8; ++i)
    {
        evaluators.push_back(new Kernel::Evaluator(t));
    }
}

Renderer::~Renderer()
{
    printf("Deleting renderer\n");
    for (auto e : evaluators)
    {
        delete e;
    }
}
