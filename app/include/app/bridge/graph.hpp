#pragma once

#include <map>

#include <QQmlEngine>
#include <QJSEngine>

#include "app/bridge/watcher.hpp"
#include "app/bridge/instance.hpp"

#include "graph/types/env.hpp"
#include "graph/response.hpp"
#include "graph/command.hpp"
#include "graph/queue.hpp"
#include "graph/root.hpp"

#include "kernel/solve/solver.hpp"

namespace App {
namespace Bridge {

class GraphModel : public QObject
{
    Q_OBJECT
public:
    GraphModel(QObject* parent=nullptr);
    ~GraphModel();

    /*
     *  Returns a SheetInstanceModel pointer
     */
    Q_INVOKABLE QObject* modelOf(QList<int> env);

    /*
     *  Applies a set of constraints to the graph
     *
     *  This should be called from the main UI thread
     */
    void setVariables(const Kernel::Solver::Solution& sol);

    /*
     *  Constructor for the QML singleton
     */
    static QObject* instance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static GraphModel* instance();

    /*
     *  Starts a root running with kernel and app bindings injected
     */
    static shared_queue<Graph::Response>& runRoot(
            Graph::Root& root,
            shared_queue<Graph::Command>& commands);

    /*
     *  Send a command to the graph thread
     */
    void enqueue(const Graph::Command& c) { commands.push(c); }

protected slots:
    void gotResponse();

    /*
     *  Update the model state with the given response
     */
    void updateFrom(const Graph::Response& r);

protected:
    std::map<Graph::Env, std::unique_ptr<SheetInstanceModel>> instances;

    Graph::Root root;
    shared_queue<Graph::Command> commands;
    shared_queue<Graph::Response>& responses;
    QueueWatcher watcher;
};

}   // namespace Bridge
}   // namespace App
