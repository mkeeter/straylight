#include "app/bridge/watcher.hpp"

namespace App {
namespace Bridge {

QueueWatcher::QueueWatcher(shared_queue<Graph::Response>& responses)
    : queue(responses)
{
    // Nothing to do here
}


void QueueWatcher::run()
{
    while (!queue.done())
    {
        queue.wait();
        emit(gotResponse());
    }
}

}   // namespace Bridge
}   // namespace App
