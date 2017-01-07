#pragma once

#include <QObject>
#include <QFuture>
#include <QFutureWatcher>

#include "kernel/eval/evaluator.hpp"
#include "kernel/tree/tree.hpp"
#include "kernel/render/heightmap.hpp"

class Renderer : public QObject
{
    Q_OBJECT
public:
    Renderer(Kernel::Tree t);
    ~Renderer();

protected:
    std::list<Kernel::Evaluator*> evaluators;
    std::atomic_bool abort;

    typedef std::pair<Kernel::DepthImage, Kernel::NormalImage> Result;
    QFuture<Result> future;
    QFutureWatcher<Result> watcher;
};
