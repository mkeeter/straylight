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
             t.size.width()/2, t.size.height()/2,
             fmax(t.size.width()/2, t.size.height()/2));

    auto inv = t.mat.inverted();

    auto scaled = inv;
    scaled.scale(1, 1, -1);
    auto m = glm::make_mat4(scaled.data());
    auto out = Kernel::Heightmap::Render(evaluators, r, abort, m);

    if (!abort.load())
    {
        // Map the depth buffer into the 0 - 1 range, with -inf = 1
        Kernel::DepthImage d =
            (out.first == -std::numeric_limits<float>::infinity())
            .select(1, (1 - out.first) / 2);
        emit(gotResult(this, {d, out.second}, inv));
    }
}
