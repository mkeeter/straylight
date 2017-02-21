#include <QObject>
#include <QQmlEngine>
#include <QQuickTextDocument>
#include <QJSEngine>

#include "app/bridge/graph.hpp"
#include "kernel/solve/solver.hpp"

namespace App {

namespace Render { class Canvas; }
namespace Bridge {

////////////////////////////////////////////////////////////////////////////////

class Bridge : public QObject
{
    Q_OBJECT
public:
    Bridge();

    /*
     *  Install a syntax highlighter to the given doc
     */
    Q_INVOKABLE void installHighlighter(QQuickTextDocument* doc);

    /*
     *  Returns a cursor position pair that matches the given position's paren
     *  (or -1, -1 if no such match exists)
     */
    Q_INVOKABLE QPoint matchedParen(QQuickTextDocument* doc, int pos);

    /*
     *  Saves to the given file, returning an error string on failure
     */
    Q_INVOKABLE QString saveFile(QUrl filename);
    Q_INVOKABLE QString loadFile(QUrl filename);
    Q_INVOKABLE void clearFile();

    /*
     *  Extracts the path from a QUrl
     */
    Q_INVOKABLE QString filePath(QUrl filename) const;

    /*
     *  Constructor for the QML singleton
     */
    static QObject* instance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static Bridge* instance();

    /*
     *  Look up the graph instances
     */
    App::Bridge::GraphModel* graph() { return _graph; }

public slots:
    /*
     *  Returns interpreter keywords
     *  (for syntax highlighting)
     */
    std::set<std::string> keywords() const { return std::set<std::string>(); }

protected:
    /*  This is owned by Qt as a QML singleton, so we heap-allocate it
     *  and don't delete it ourselves  */
    App::Bridge::GraphModel* _graph;

    static Bridge* _instance;
};

}   // namespace Bridge
}   // namespace App
