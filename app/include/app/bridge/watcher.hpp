#pragma once

#include <QObject>
#include <QThread>

#include "graph/queue.hpp"
#include "graph/response.hpp"

namespace App {
namespace Bridge {

class QueueWatcher : public QThread
{
    Q_OBJECT
public:
    QueueWatcher(shared_queue<Graph::Response>& responses);

signals:
    void gotResponse();

protected:
    void run() override;

    shared_queue<Graph::Response>& queue;
};

}   // namespace Bridge
}   // namespace App
