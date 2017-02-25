#pragma once

#include <map>

#include <QQmlEngine>
#include <QJSEngine>
#include <QRegularExpression>
#include <QSet>

#include "app/bridge/watcher.hpp"
#include "app/bridge/sheet.hpp"

#include "graph/types/env.hpp"
#include "graph/response.hpp"
#include "graph/command.hpp"
#include "graph/queue.hpp"
#include "graph/async.hpp"

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
            Graph::AsyncRoot& root,
            shared_queue<Graph::Command>& commands);

    /*
     *  Send a command to the graph thread
     */
    void enqueue(const Graph::Command& c) { commands.push(c); }

    /*
     *  Checks that the given string isn't a reserved word or invalid
     */
    QString isValidItemName(QString s) const;
    QString isValidSheetName(QString s) const;

    /*
     *  Returns the reported set of keywords
     */
    const QSet<QString>& getKeywords() const { return keywords; }

    /*
     *  Clears the graph
     */
    void clear();

signals:
    /*
     *  Announces that a particular instance has been erased
     *
     *  The SheetStack should listen to this signal and close to before that
     *  instance if it's currently open
     */
    void instanceErased(unsigned i);

protected slots:
    void gotResponse();

    /*
     *  Update the model state with the given response
     */
    void updateFrom(const Graph::Response& r);

protected:
    std::map<Graph::SheetIndex, std::unique_ptr<SheetModel>> sheets;
    std::map<Graph::InstanceIndex, Graph::SheetIndex> instances;

    Graph::AsyncRoot root;
    shared_queue<Graph::Command> commands;
    shared_queue<Graph::Response>& responses;
    QueueWatcher watcher;

    /*  Reserved words from the interpreter  */
    QSet<QString> keywords;
    QList<QPair<QRegularExpression, QString>> bad_item_names;
    QList<QPair<QRegularExpression, QString>> bad_sheet_names;
    QRegularExpression good_item_name;
    QRegularExpression good_sheet_name;
};

}   // namespace Bridge
}   // namespace App
