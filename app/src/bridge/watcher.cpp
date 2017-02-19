#include "app/bridge/watcher.hpp"

namespace App {
namespace Bridge {

QueueWatcher::QueueWatcher(shared_queue<Graph::Response>& responses)
    : queue(responses)
{
    moveToThread(&thread);
    connect(&thread, &QThread::started, this, &QueueWatcher::run);
    thread.start();
}

void QueueWatcher::run()
{
    while (true)
    {
        queue.wait();
        if (queue.peek().op == Graph::Response::HALT)
        {
            break;
        }
        else
        {
            emit(gotResponse());
        }
    }
}

}   // namespace Bridge
}   // namespace App
