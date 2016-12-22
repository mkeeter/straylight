#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>

#include "graph/root.hpp"
#include "graph/serializer.hpp"

class Bridge : public QObject
{
    Q_OBJECT
public:
    Bridge() : bts(this) { /* Nothing to do here */ }

    /*
     *  Checks a name for creation / renaming
     */
    Q_INVOKABLE bool checkName(QString name) const;

    /*
     *  Constructor for the QML singleton
     */
    static QObject* singleton(QQmlEngine *engine, QJSEngine *scriptEngine);
    static Root* root();

    /*
     *  Requests that the UI be synchronized to the graph
     */
    void sync();

signals:
    /*
     *  Set of signals for deserialization of the graph
     *  (interpreted as a tree)
     */
    void beginSheet(int s);
    void endSheet();
    void instance(int i, const QString& name);
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

        bool beginSheet(SheetIndex s) override;
        void endSheet() override;
        void instance(InstanceIndex i, const std::string& name) override;
        void input(CellIndex c, const std::string& name,
                   const std::string& expr, bool valid,
                   const std::string& val) override;
        void output(CellIndex c, const std::string& name,
                    bool valid, const std::string& val) override;
        void cell(CellIndex c, const std::string& name,
                  const std::string& expr, Cell::Type type,
                  bool valid, const std::string& val) override;
    private:
        Bridge* parent;
    };

    BridgeTreeSerializer bts;

    static Bridge* _instance;
};
