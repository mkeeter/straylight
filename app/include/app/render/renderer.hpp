#pragma once

#include <QObject>
#include <QVector2D>
#include <QMatrix4x4>
#include <QFuture>
#include <QFutureWatcher>

#include "kernel/eval/evaluator.hpp"
#include "kernel/tree/tree.hpp"
#include "kernel/render/heightmap.hpp"

namespace App {
namespace Render {

class Renderer : public QObject
{
    Q_OBJECT
public:
    Renderer(Kernel::Evaluator* e);
    ~Renderer();

    /*
     *  Asks this renderer to delete itself when it's done
     *  with the current task.
     */
    void deleteWhenNotRunning();

    /*
     *  Updates each of the stored Evaluators with changed
     *  variable values, return true if there's a change.
     */
    bool updateVars(Kernel::Tree tree);

    /*
     *  This struct can be passed into a Blitter to draw
     */
    struct Result {
        Result(Kernel::DepthImage* d, Kernel::NormalImage* n,
               QMatrix4x4 M) : depth(d), norm(n), M(M) {}
        std::unique_ptr<Kernel::DepthImage> depth;
        std::unique_ptr<Kernel::NormalImage> norm;
        QMatrix4x4 M;
    };

    /*
     *  This represents a single render task
     */
    struct Task {
        Task() {}
        Task(QMatrix4x4 mat, QSize size, int level)
            : mat(mat), size(size), level(level) {}

        QMatrix4x4 mat;
        QSize size;
        int level;
    };

    /*
     *  Marks that the view has changed and we should re-render
     */
    void enqueue(QMatrix4x4 mat, QSize size);

    /*
     *  Returns the current result, claiming it and setting it to nullptr
     */
    Result* getResult();

    /*
     *  Activates the renderer, kicking off a render operation if there's one
     */
    void activate();

    /*
     *  Deactivates the renderer
     */
    void deactivate();

signals:
    /*
     *  Emitted when a render operation is done
     */
    void done();

protected slots:
    void onRenderFinished();

protected:
    /*
     *  Kicks off a new render operation if we're not rendering
     *  and next task is available
     */
    void checkNext();

    /*
     *  Run the rendering task on the given task
     */
    void run(Task t);

    std::vector<Kernel::Evaluator*> evaluators;
    std::atomic_bool abort;
    int base_level=4;

    /*  What should we do when the current task completes?
     *  (this would be nicer as a variant type) */
    Task next;
    enum { NOTHING, NEXT, DELETE } todo;
    bool active = true;
    std::mutex todo_lock;

    QFuture<void> future;
    QFutureWatcher<void> watcher;

    std::mutex result_lock;
    std::unique_ptr<Result> result;
};

}   // namespace Render
}   // namespace App
