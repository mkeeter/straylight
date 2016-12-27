#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>

#include "graph/root.hpp"
#include "graph/serializer.hpp"

class Bridge : public QObject
{
    Q_OBJECT
public:
    Bridge();

    /*
     *  Checks whether a name is valid
     *  Returns an empty string on success and an error message otherwise
     */
    Q_INVOKABLE QString checkName(int sheet_index, QString name) const;
    Q_INVOKABLE QString checkRename(int item_index, QString name) const;
    Q_INVOKABLE void renameItem(int item_index, QString name);

    Q_INVOKABLE void insertCell(int sheet_index, const QString& name);
    Q_INVOKABLE void setExpr(int cell_index, const QString& expr);
    Q_INVOKABLE void setInput(int instance_index, int cell_index,
                              const QString& expr);
    Q_INVOKABLE void eraseCell(int cell_index);
    Q_INVOKABLE void eraseInstance(int instance_index);

    Q_INVOKABLE int sheetOf(int instance_index) const;
    Q_INVOKABLE QString nextItemName(int sheet_index) const;

    /*
     *  Constructor for the QML singleton
     */
    static QObject* singleton(QQmlEngine *engine, QJSEngine *scriptEngine);
    static Root* root();

    /*
     *  Requests that the UI be synchronized to the graph
     */
    Q_INVOKABLE void sync();

signals:
    /*
     *  Set of signals for deserialization of the graph
     *  (interpreted as a tree)
     */
    void push(const QString& instance_name, const QString& sheet_name);
    void pop();

    void instance(int i, const QString& name, const QString& sheet);
    void input(int c, const QString& name,
               const QString& expr, bool valid,
               const QString& val);
    void output(int c, const QString& name,
                bool valid, const QString& val);
    void cell(int c, const QString& name,
              const QString& expr, int type,
              bool valid, const QString& val);

protected:
    Root r;

    /*
     *  Lightweight TreeSerializer class
     *  (passes everything back to its parent signals)
     */
    class BridgeTreeSerializer : public TreeSerializer
    {
    public:
        BridgeTreeSerializer(Bridge* b) : parent(b) {}

        void cell(CellIndex c, const std::string& name,
                  const std::string& expr, Cell::Type type,
                  bool valid, const std::string& val) override;

        void instance(InstanceIndex i, const std::string& name,
                      const std::string& sheet) override;
        void input(CellIndex c, const std::string& name,
                   const std::string& expr, bool valid,
                   const std::string& val) override;
        void output(CellIndex c, const std::string& name,
                    bool valid, const std::string& val) override;

        bool push(const std::string& instance_name,
                  const std::string& sheet_name) override;
        void pop() override;
    private:
        Bridge* parent;
    };

    BridgeTreeSerializer bts;

    static Bridge* _instance;
};
