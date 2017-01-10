#include <QtConcurrent>
#include <QThread>

#include <glm/gtc/type_ptr.hpp>

#include "renderer.hpp"

Renderer::Renderer(Kernel::Tree t)
    : todo(NOTHING)
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
    assert(!watcher.isRunning());

    for (auto e : evaluators)
    {
        delete e;
    }

    emit(goodbye(this));
}

void Renderer::deleteWhenNotRunning()
{
    if (!watcher.isRunning())
    {
        delete this;
    }
    else
    {
        abort.store(true);
        todo = DELETE;
    }
}

void Renderer::onViewChanged(QMatrix4x4 mat, QSize size)
{
    if (todo != DELETE)
    {
        next = Task(mat, size);
        todo = NEXT;

        abort.store(true);
        checkNext();
    }
}

void Renderer::onRenderFinished()
{
    if (todo == DELETE)
    {
        delete this;
    }
    else
    {
        checkNext();
    }
}

void Renderer::checkNext()
{
    if (todo == NEXT && !watcher.isRunning())
    {
        todo = NOTHING;

        abort.store(false);
        future = QtConcurrent::run(this, &Renderer::run, next);
        watcher.setFuture(future);
    }
}

void Renderer::run(Task t)
{
    Kernel::Region r({-1, 1}, {-1, 1}, {-1, 1},
             t.size.width()/2,
             t.size.height()/2, 255);
    auto m = glm::make_mat4(t.mat.inverted().data());
    auto out = Kernel::Heightmap::Render(evaluators, r, abort, m);

    emit(gotResult(this, {out.first, out.second}, t));
}
