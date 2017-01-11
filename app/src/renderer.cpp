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
    qDebug() << "Renderer::onViewChanged" << QThread::currentThread();
    if (todo != DELETE)
    {
        next = Task(mat, size);
        todo = NEXT;

        abort.store(true);
        qDebug() << "   checking";
        checkNext();
    }
}

void Renderer::onRenderFinished()
{
    qDebug() << "Renderer::onRenderFinished" << QThread::currentThread();
    if (todo == DELETE)
    {
        qDebug() << "   deleting";
        delete this;
    }
    else
    {
        qDebug() << "   checking";
        checkNext();
    }
}

void Renderer::checkNext()
{
    qDebug() << "   Renderer::checkNext" << QThread::currentThread();
    if (todo == NEXT && !watcher.isRunning())
    {
        todo = NOTHING;

        abort.store(false);
        qDebug() << "     running!";
        future = QtConcurrent::run(this, &Renderer::run, next);
        watcher.setFuture(future);
    }
}

void Renderer::run(Task t)
{
    qDebug() << "Renderer::run" << QThread::currentThread();
    Kernel::Region r({-1, 1}, {-1, 1}, {-1, 1},
             t.size.width()/2,
             t.size.height()/2, 255);

    auto inv = t.mat.inverted();

    auto scaled = inv;
    scaled.scale(1, 1, -1);
    auto m = glm::make_mat4(scaled.data());
    auto out = Kernel::Heightmap::Render(evaluators, r, abort, m);

    Kernel::Image::SavePng("/Users/mkeeter/Desktop/out.png", out.first.colwise().reverse());
    qDebug() << "  emitting gotResult";
    emit(gotResult(this, {out.first, out.second}, inv));
    qDebug() << "  done";
}
