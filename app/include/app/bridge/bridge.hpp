#include <QObject>
#include <QQmlEngine>
#include <QQuickTextDocument>
#include <QJSEngine>

#include "app/bridge/graph.hpp"
#include "kernel/solve/solver.hpp"

namespace App {

namespace Render { class Canvas; }
namespace Core {

class UndoStack;

////////////////////////////////////////////////////////////////////////////////

class Bridge : public QObject
{
    Q_OBJECT
public:
    Bridge();

    /*
     *  Checks whether a name is valid
     *  Returns an empty string on success and an error message otherwise
     */
    Q_INVOKABLE QString checkItemName(int sheet_index, QString name) const;
    Q_INVOKABLE QString checkItemRename(int item_index, QString name) const;
    Q_INVOKABLE void renameItem(int item_index, QString name);
    Q_INVOKABLE QString nextItemName(int sheet_index) const;

    Q_INVOKABLE QString checkSheetName(int parent_sheet, QString name) const;
    Q_INVOKABLE QString checkSheetRename(int sheet_index, QString name) const;
    Q_INVOKABLE void renameSheet(int sheet_index, QString name);
    Q_INVOKABLE void insertSheet(int sheet_index, QString name);
    Q_INVOKABLE QString nextSheetName(int sheet_index) const;
    Q_INVOKABLE void eraseSheet(int sheet_index);

    Q_INVOKABLE void insertCell(int sheet_index, const QString& name);
    Q_INVOKABLE void setExpr(int cell_index, const QString& expr);
    Q_INVOKABLE void setInput(int instance_index, int cell_index,
                              const QString& expr);
    Q_INVOKABLE void eraseCell(int cell_index);

    Q_INVOKABLE void insertInstance(int parent_sheet_index, QString name,
                                    int target_sheet_index);
    Q_INVOKABLE void eraseInstance(int instance_index);

    Q_INVOKABLE int sheetOf(int instance_index) const;

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
     *  Applies a set of constraints to the graph
     *
     *  This should be called from the main UI thread
     */
    void setVariables(const Kernel::Solver::Solution& sol);

public slots:
    /*
     *  Returns interpreter keywords
     *  (for syntax highlighting)
     */
    std::set<std::string> keywords() const { return std::set<std::string>(); }

protected:
    App::Bridge::GraphModel graph;

    static Bridge* _instance;
};

}   // namespace Core
}   // namespace App
