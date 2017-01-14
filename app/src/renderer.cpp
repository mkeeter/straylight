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
        next = Task(mat, size, base_level);
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
    if (!watcher.isRunning())
    {
        // Load refinement render task
        if (todo == NOTHING && next.level > 0)
        {
            next.level--;
            todo = NEXT;
        }

        if (todo == NEXT)
        {
            todo = NOTHING;

            abort.store(false);
            future = QtConcurrent::run(this, &Renderer::run, next);
            watcher.setFuture(future);
        }
    }
}

void Renderer::run(Task t)
{
    auto start_time = QTime();
    start_time.start();

    const float rx = t.size.width() * 2 / (1 << t.level);
    const float ry = t.size.height() * 2 / (1 << t.level);
    Kernel::Region r({-1, 1}, {-1, 1}, {-1, 1}, rx, ry, fmax(rx, ry));

    auto inv = t.mat.inverted();

    // Flip Z axis for correct rendering
    auto scaled = inv;
    scaled.scale(1, 1, -1);
    auto m = glm::make_mat4(scaled.data());

    // Make rendering at the base level un-abortable
    // (otherwise we'd have a bunch of black images when rotating)
    std::atomic_bool dummy(false);
    std::atomic_bool& abort_ = (t.level == base_level) ? dummy : abort;

    auto out = Kernel::Heightmap::Render(evaluators, r, abort_, m);

    if (!abort_.load())
    {
        // Map the depth buffer into the 0 - 1 range, with -inf = 1
        Kernel::DepthImage d =
            (out.first == -std::numeric_limits<float>::infinity())
            .select(1, (1 - out.first) / 2);
        d = (out.first == r.Z.values.back()).select(0, d);
        emit(gotResult(this, {d, out.second}, inv));

        auto dt = start_time.elapsed();
        if (dt < 10 & base_level > 0)
        {
            //base_level--;
        }
        else if (dt > 20)
        {
            //base_level++;
        }
    }
}
