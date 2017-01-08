#pragma once

#include <QObject>
#include <QVector2D>
#include <QMatrix4x4>
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

public slots:
    void onViewChanged(QMatrix4x4 mat, QSize size);

protected:
    struct Result {
        Kernel::DepthImage depth;
        Kernel::NormalImage norm;
    };

    struct Task {
        Task() : valid(false) {}
        Task(QMatrix4x4 mat, QSize size)
            : mat(mat), size(size), valid(true) {}

        QMatrix4x4 mat;
        QSize size;
        bool valid=false;
    };

    /*
     *  Kicks off a new render operation, drawing from next_task
     */
    void startRender();

    /*
     *  Run the rendering task on the current task
     */
    Result run();

protected slots:
    void onRenderFinished();

protected:
    std::vector<Kernel::Evaluator*> evaluators;
    std::atomic_bool abort;

    Task current_task;
    Task next_task;

    QFuture<Result> future;
    QFutureWatcher<Result> watcher;
};
