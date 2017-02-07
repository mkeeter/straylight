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
    Renderer(Kernel::Tree t);
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

    struct Result {
        Kernel::DepthImage* depth;
        Kernel::NormalImage* norm;
    };

    struct Task {
        Task() {}
        Task(QMatrix4x4 mat, QSize size, int level)
            : mat(mat), size(size), level(level) {}

        QMatrix4x4 mat;
        QSize size;
        int level;
    };

signals:
    /*
     *  Passes off a result to the blitter
     */
    void gotResult(Renderer* self, const Result r, const QMatrix4x4& mat);

    /*
     *  Emitted before (self-)destruction
     *  Used to inform the blitter that these results should be forgotten
     */
    void goodbye(Renderer* self);

public slots:
    void onViewChanged(QMatrix4x4 mat, QSize size);

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

    QFuture<void> future;
    QFutureWatcher<void> watcher;
};

}   // namespace Render
}   // namespace App
