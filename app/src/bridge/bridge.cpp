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
    // Nothing to do here
}

void Bridge::installHighlighter(QQuickTextDocument* doc)
{
    new App::UI::SyntaxHighlighter(doc->textDocument());
}

QPoint Bridge::matchedParen(QQuickTextDocument* doc, int pos)
{
    return App::UI::SyntaxHighlighter::matchedParen(doc->textDocument(), pos);
}

QString Bridge::saveFile(QUrl filename)
{
    QFile file(filename.toLocalFile());
    if (!file.open(QIODevice::WriteOnly))
    {
        return file.errorString();
    }

#if 0   // TODO
    auto str = r.getTree().toString();
    file.write(str.data(), str.size());
    undo_stack->setClean();
#endif
    return "";
}

QString Bridge::loadFile(QUrl filename)
{
    QFile file(filename.toLocalFile());
    if (!file.open(QIODevice::ReadOnly))
    {
        return file.errorString();
    }

    auto str = file.readAll();
#if 0   // TODO
    auto out = QString::fromStdString(r.loadString(str.toStdString()));
    sync();
    undo_stack->clear();
    return out;
#endif
    return "";
}

void Bridge::clearFile()
{
#if 0
    r.clear();
    undo_stack->clear();
    sync();
#endif
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
