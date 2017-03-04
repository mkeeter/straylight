#include <QThread>

#include "app/bridge/bridge.hpp"
#include "app/render/canvas.hpp"
#include "app/ui/syntax.hpp"

namespace App {
namespace Bridge {

Bridge* Bridge::_instance = nullptr;

Bridge::Bridge()
    : _graph(new GraphModel())
{
    connect(_graph, &GraphModel::serialized,
            this, &Bridge::onSerialized);
    connect(_graph, &GraphModel::deserialized,
            this, &Bridge::onDeserialized);
}

void Bridge::installHighlighter(QQuickTextDocument* doc)
{
    new App::UI::SyntaxHighlighter(doc->textDocument(), _graph->getKeywords());
}

QPoint Bridge::matchedParen(QQuickTextDocument* doc, int pos)
{
    return App::UI::SyntaxHighlighter::matchedParen(doc->textDocument(), pos);
}

////////////////////////////////////////////////////////////////////////////////

void Bridge::saveFile(QUrl f)
{
    filename = f;

    // Kick off async serialization
    _graph->serialize();
}

void Bridge::onSerialized(QString expr)
{
    QFile file(filename.toLocalFile());
    if (!file.open(QIODevice::WriteOnly))
    {
        emit(serializeError(file.errorString()));
    }
    else
    {
        QTextStream out(&file);
        out << expr;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Bridge::loadFile(QUrl filename)
{
    QFile file(filename.toLocalFile());
    if (!file.open(QIODevice::ReadOnly))
    {
        emit(deserializeError(file.errorString()));
    }
    else
    {
        auto str = file.readAll();
        _graph->deserialize(str);
    }
}

void Bridge::onDeserialized(QString err)
{
    if (err.size())
    {
        emit(deserializeError(err));
    }
}

////////////////////////////////////////////////////////////////////////////////

void Bridge::clearFile()
{
    _graph->clear();
}

QString Bridge::filePath(QUrl filename) const
{
    return filename.toLocalFile();
}

QObject* Bridge::instance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return instance();
}

Bridge* Bridge::instance()
{
    if (_instance == nullptr)
    {
        _instance = new Bridge();
    }
    return _instance;
}

}   // namespace Bridge
}   // namespace App
