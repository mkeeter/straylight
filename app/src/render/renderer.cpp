#include <QtConcurrent>
#include <QThread>

#include <glm/gtc/type_ptr.hpp>

#include "app/render/renderer.hpp"

namespace App {
namespace Render {

Renderer::Renderer(Kernel::Tree e, std::map<Kernel::Tree::Tree_*, float> vars)
    : next(QMatrix4x4(), {0,0}, 0), todo(NOTHING)
{
    for (int i=0; i < 8; ++i)
    {
        evaluators.push_back(new Kernel::Evaluator(e, vars));
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
}

void Renderer::deleteWhenNotRunning()
{
    std::lock_guard<std::mutex> lock(todo_lock);
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

// TODO: this is never used
bool Renderer::updateVars(std::map<Kernel::Tree::Tree_*, float> vars)
{
    bool changed = false;
    for (auto& e : evaluators)
    {
        changed |= e->updateVars(vars);
    }
    return changed;
}

void Renderer::enqueue(QMatrix4x4 mat, QSize size)
{
    std::lock_guard<std::mutex> lock(todo_lock);
    if (todo != DELETE)
    {
        // Make rendering at the base level un-abortable
        // (otherwise we'd have a bunch of black images when rotating)
        if (watcher.isRunning() && next.level != base_level)
        {
            abort.store(true);
        }

        next = Task(mat, size, base_level);
        todo = NEXT;
        checkNext();
    }
}

Renderer::Result* Renderer::getResult()
{
    std::lock_guard<std::mutex> lock(result_lock);
    return result.release();
}

void Renderer::onRenderFinished()
{
    std::lock_guard<std::mutex> lock(todo_lock);
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

        if (todo == NEXT && active)
        {
            todo = NOTHING;

            abort.store(false);
            future = QtConcurrent::run(this, &Renderer::run, next);
            watcher.setFuture(future);
        }
    }
}

void Renderer::activate()
{
    std::lock_guard<std::mutex> lock(todo_lock);
    active = true;
    checkNext();
}

void Renderer::deactivate()
{
    active = false;
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

    auto out = Kernel::Heightmap::render_(evaluators, r, abort, m);

    if (!abort.load())
    {
        // Map the depth buffer into the 0 - 1 range, with -inf = 1
        *out.first =
            (*out.first == -std::numeric_limits<float>::infinity())
            .select(1, (1 - *out.first) / 2);

        // Then map the back Z value to 0
        *out.first = (*out.first == r.Z.values.back()).select(0, *out.first);

        {   // Store the result, safe against other threads
            std::lock_guard<std::mutex> lock(result_lock);
            result.reset(new Result(out.first, out.second, inv));
        }
        emit(done());

        auto dt = start_time.elapsed();
        if (dt < 10 & base_level > 0)
        {
            base_level--;
        }
        else if (dt > 20)
        {
            base_level++;
        }
    }
}

}   // namespace Render
}   // namespace App
