#pragma once

#include <QObject>
#include <QThread>

#include "graph/queue.hpp"
#include "graph/response.hpp"

namespace App {
namespace Bridge {

class QueueWatcher : public QObject
{
    Q_OBJECT
public:
    QueueWatcher(shared_queue<Graph::Response>& responses);
signals:
    void gotResponse();
protected slots:
    void run();

protected:
    shared_queue<Graph::Response>& queue;
    QThread thread;
};

}   // namespace Bridge
}   // namespace App
