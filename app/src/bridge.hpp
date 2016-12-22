#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>

#include "graph/root.hpp"
#include "graph/serializer.hpp"

class Bridge : public QObject, TreeSerializer
{
    Q_OBJECT
public:
    Q_INVOKABLE bool checkName(QString name) const;

    static QObject* singleton(QQmlEngine *engine, QJSEngine *scriptEngine);

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

protected:
    Root graph;
};
