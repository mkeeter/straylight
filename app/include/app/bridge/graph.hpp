#pragma once

#include <map>

#include "app/bridge/watcher.hpp"
#include "app/bridge/instance.hpp"

#include "graph/types/env.hpp"
#include "graph/response.hpp"
#include "graph/command.hpp"
#include "graph/queue.hpp"
#include "graph/root.hpp"

namespace App {
namespace Bridge {

class GraphModel : public QObject
{
    Q_OBJECT
public:
    GraphModel(QObject* parent=nullptr);

    /*
     *  Update the model state with the given response
     */
    void updateFrom(const Graph::Response& r);

protected slots:
    void gotResponse();

protected:
    std::map<Graph::Env, std::unique_ptr<SheetInstanceModel>> instances;

    Graph::Root root;
    shared_queue<Graph::Command> commands;
    shared_queue<Graph::Response>& responses;
    QueueWatcher watcher;
};

}   // namespace Bridge
}   // namespace App
