#include <QObject>
#include <QQmlEngine>
#include <QQuickTextDocument>
#include <QJSEngine>

#include "graph/root.hpp"
#include "graph/serializer.hpp"

// Forward declaration of Canvas
class Canvas;

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
     *  Sets the pinged variable to true
     *
     *  This is used when the Bridge tells the UI about an instance, and the
     *  UI tells the Bridge whether that instance is being drawn.
     */
    Q_INVOKABLE void ping() { pinged = true; }

    /*
     *  Saves to the given file, returning an error string on failure
     */
    Q_INVOKABLE QString saveFile(QUrl filename);
    Q_INVOKABLE QString loadFile(QUrl filename);
    Q_INVOKABLE void clearFile();

    /*
     *  Constructor for the QML singleton
     */
    static QObject* singleton(QQmlEngine *engine, QJSEngine *scriptEngine);
    static Bridge* singleton();

    /*
     *  Extract the graph root from the singleton
     */
    static Graph::Root* root();

    /*
     *  Requests that the UI be synchronized to the graph
     *
     *  This should be called from the UI thread when things change
     */
    Q_INVOKABLE void sync();

    /*
     *  Installs the Canvas object
     */
    void attachCanvas(Canvas* c);

signals:
    /*
     *  Set of signals for deserialization of the graph
     *  (interpreted as a tree)
     *
     *  These signals are connected directly to the UI, which is responsible
     *  for maintaining serializer state and updating its models
     */
    void push(const int instance_index, const QString& instance_name,
              const QString& sheet_name);
    void pop();

    void instance(int i, const QString& name, const QString& sheet);
    void input(int c, const QString& name, const QString& expr, bool valid,
               const QString& val);
    void output(int c, const QString& name, bool valid, const QString& val);

    // ptr is actually of type Graph::ValuePtr, but we use void* here due to
    // Qt limitations (since it doesn't let you pass ValuePtrs through slots)
    void cell(int c, const QString& name, const QString& expr, int type,
              bool valid, const QString& val, void* ptr);

    void sheet(int s, const QString& name, bool editable, bool insertable);

    /*
     *  Used to queue a sync from a non-UI thread
     */
    void syncLater();

public slots:
    /*
     *  Returns interpreter keywords
     *  (for syntax highlighting)
     */
    std::set<std::string> keywords() const { return r.keywords(); }

protected:
    Graph::Root r;

    /*
     *  Lightweight TreeSerializer class
     *  (passes everything back to its parent signals)
     */
    class BridgeTreeSerializer : public Graph::TreeSerializer
    {
    public:
        BridgeTreeSerializer(Bridge* b) : parent(b) {}

        void cell(Graph::CellIndex c, const std::string& name,
                  const std::string& expr, Graph::Cell::Type type,
                  bool valid, const std::string& val,
                  Graph::ValuePtr ptr) override;

        void instance(Graph::InstanceIndex i, const std::string& name,
                      const std::string& sheet) override;
        void input(Graph::CellIndex c, const std::string& name,
                   const std::string& expr, bool valid,
                   const std::string& val) override;
        void output(Graph::CellIndex c, const std::string& name,
                    bool valid, const std::string& val) override;

        bool push(Graph::InstanceIndex i, const std::string& instance_name,
                  const std::string& sheet_name) override;
        void pop() override;

        void sheet(Graph::SheetIndex s, const std::string& sheet_name,
                   bool editable, bool insertable) override;
    private:
        Bridge* parent;
    };

    /*  Permanent serializer that's connected to the right places  */
    BridgeTreeSerializer bts;
    bool pinged=false;

    static Bridge* _instance;
};
