#include <QtConcurrent>
#include <QThread>

#include <glm/gtc/type_ptr.hpp>

#include "renderer.hpp"

Renderer::Renderer(Kernel::Tree t)
{
    for (int i=0; i < 8; ++i)
    {
        evaluators.push_back(new Kernel::Evaluator(t));
    }
    connect(&watcher, &QFutureWatcher<Result>::finished,
            this, &Renderer::onRenderFinished);
}

Renderer::~Renderer()
{
    for (auto e : evaluators)
    {
        delete e;
    }
}

void Renderer::onViewChanged(QMatrix4x4 mat, QSize size)
{
    next_task = Task(mat, size);
    if (!watcher.isRunning())
    {
        printf("Starting new render from onViewChanged!\n");
        startRender();
    }
    else
    {
        abort.store(true);
    }
}

void Renderer::onRenderFinished()
{
    // Do something with result here
    printf("Got result!\n");

    if (next_task.valid)
    {
        printf("Starting new render from onRenderFinished!\n");
        startRender();
    }
}

void Renderer::startRender()
{
    assert(next_task.valid);
    assert(!watcher.isRunning());

    current_task = next_task;
    next_task = Task(); // marked as invalid

    abort.store(false);
    future = QtConcurrent::run(this, &Renderer::run);
    watcher.setFuture(future);
}

Renderer::Result Renderer::run()
{
    Kernel::Region r({-1, 1}, {-1, 1}, {-1, 1},
             current_task.size.width()/2,
             current_task.size.height()/2, 255);
    auto m = glm::make_mat4(current_task.mat.data());
    auto out = Kernel::Heightmap::Render(evaluators, r, abort, m);
    return {out.first, out.second};
}
