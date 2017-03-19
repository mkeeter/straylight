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

#include "kernel/tree/cache.hpp"
#include "kernel/solve/solver.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace App {
namespace Render { class Scene; }
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

    /*
     *  Pushes a SERIALIZED response down the pipe
     */
    void serialize();

    /*
     *  Requests deserialization of the given string
     */
    void deserialize(const QString& s);

    /*
     *  Installs a Scene
     *  (should be called from Scene constructor)
     */
    void installScene(App::Render::Scene* s) { scene = s; }

    /*
     *  Manipulate the mapping of cell keys (in the Graph)
     *  to variable pointers (in the Kernel's tree).
     */
    void defineVar(const Graph::CellKey& k,
                   const Kernel::Tree::Id var);
    void forgetVar(const Kernel::Tree::Id var);

signals:
    /*
     *  Announces that a particular instance has been erased
     *
     *  The SheetStack should listen to this signal and close to before that
     *  instance if it's currently open
     */
    void instanceErased(unsigned i);

    /*
     *  Emitted to finish a save operation
     */
    void serialized(QString expr);

    /*
     *  Emitted to mark that a load was successful
     */
    void deserialized(QString error);

protected slots:
    /*
     *  When the viewport env changes, this is called to change which
     *  handles are drawn.
     */
    void viewEnvChanged(QList<int> env);

    /*
     *  Used to signal a response from the response watcher thread
     */
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

    App::Render::Scene* scene;

    /*  Bindings from variables to cells  */
    std::map<const Kernel::Tree::Id, Graph::CellKey> vars;

    /*  Reserved words from the interpreter  */
    QSet<QString> keywords;
    QList<QPair<QRegularExpression, QString>> bad_item_names;
    QList<QPair<QRegularExpression, QString>> bad_sheet_names;
    QRegularExpression good_item_name;
    QRegularExpression good_sheet_name;
};

}   // namespace Bridge
}   // namespace App
