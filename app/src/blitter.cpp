#include "blitter.hpp"

void Blitter::addQuad(Renderer* R, Renderer::Result imgs, Renderer::Task t)
{
    quads[R] = Quad();
}

void Blitter::forget(Renderer* R)
{
    quads.erase(R);
}
