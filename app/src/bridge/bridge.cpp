#include <QThread>

#include "core/bridge.hpp"
#include "core/undo.hpp"
#include "render/canvas.hpp"
#include "ui/syntax.hpp"

#include "bind/bind_s7.h"
#include "kernel/bind/bind_s7.h"

namespace App {
namespace Core {

Bridge* Bridge::_instance = nullptr;

Bridge::Bridge()
    : bts(this), undo_stack(UndoStack::singleton())
{
    // Inject the kernel bindings into the interpreter
    r.call(Kernel::Bind::init);
    r.call(App::Bind::init);

    // Connect async call to sync
    connect(this, &Bridge::syncLater,
            this, &Bridge::sync, Qt::QueuedConnection);
}

QString Bridge::checkItemName(int sheet_index, QString name) const
{
    std::string out;
    r.checkItemName(sheet_index, name.toStdString(), &out);
    return QString::fromStdString(out);
}

QString Bridge::checkItemRename(int item_index, QString name) const
{
    const auto& current_name = r.getTree().nameOf(item_index);
    if (current_name == name.toStdString())
    {
        return "";
    }

    auto sheet = r.getTree().parentOf(item_index);
    return checkItemName(sheet.i, name);
}

void Bridge::renameItem(int item_index, QString name)
{
    const auto n = name.toStdString();
    if (n != r.getTree().nameOf(item_index))
    {
        auto c = Checkpoint("(rename item)");
        r.renameItem(item_index, n);
    }
}

////////////////////////////////////////////////////////////////////////////////

QString Bridge::checkSheetName(int parent_sheet, QString name) const
{
    std::string out;
    r.checkSheetName(parent_sheet, name.toStdString(), &out);
    return QString::fromStdString(out);
}

QString Bridge::checkSheetRename(int sheet_index, QString name) const
{
    const auto& current_name = r.getTree().nameOf(sheet_index);
    if (current_name == name.toStdString())
    {
        return "";
    }

    auto sheet = r.getTree().parentOf(sheet_index);
    return checkSheetName(sheet.i, name);
}

void Bridge::renameSheet(int sheet_index, QString name)
{
    const auto n = name.toStdString();
    if (n != r.getTree().nameOf(sheet_index))
    {
        auto c = Checkpoint("(rename sheet)");
        r.renameSheet(sheet_index, name.toStdString());
    }
}

void Bridge::insertSheet(int sheet_index, QString name)
{
    auto c = Checkpoint("(insert sheet)");
    r.insertSheet(sheet_index, name.toStdString());
}

void Bridge::eraseSheet(int sheet_index)
{
    auto c = Checkpoint("(erase sheet)");
    r.eraseSheet(Graph::SheetIndex(sheet_index));
}

////////////////////////////////////////////////////////////////////////////////

void Bridge::insertCell(int sheet_index, const QString& name)
{
    auto c = Checkpoint("(insert cell)");
    r.insertCell(sheet_index, name.toStdString());
}

void Bridge::setExpr(int cell_index, const QString& expr)
{
    // We don't use Checkpoint here, because this is called on every
    // character when we only want to set undo/redo when the document
    // changes substantially.
    if (r.setExpr(cell_index, expr.toStdString()))
    {
        sync();
    }
}

void Bridge::setInput(int instance_index, int cell_index,
                      const QString& expr)
{
    if (r.setInput(Graph::InstanceIndex(instance_index),
                   Graph::CellIndex(cell_index),
                   expr.toStdString()))
    {
        sync();
    }
}

void Bridge::eraseCell(int cell_index)
{
    auto c = Checkpoint("(erase cell)");
    r.eraseCell(cell_index);
}

void Bridge::insertInstance(int parent_sheet_index, QString name,
                            int target_sheet_index)
{
    auto c = Checkpoint("(insert instance)");
    r.insertInstance(Graph::SheetIndex(parent_sheet_index), name.toStdString(),
                     Graph::SheetIndex(target_sheet_index));
}

void Bridge::eraseInstance(int instance_index)
{
    auto c = Checkpoint("(erase instance)");
    r.eraseInstance(Graph::InstanceIndex(instance_index));
}

int Bridge::sheetOf(int instance_index) const
{
    return r.getTree().at(
            Graph::InstanceIndex(instance_index)).instance()->sheet.i;
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

    auto str = r.getTree().toString();
    file.write(str.data(), str.size());
    undo_stack->setClean();
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
    auto out = QString::fromStdString(r.loadString(str.toStdString()));
    sync();
    undo_stack->clear();
    return out;
}

void Bridge::clearFile()
{
    r.clear();
    undo_stack->clear();
    sync();
}

QString Bridge::filePath(QUrl filename) const
{
    return filename.toLocalFile();
}

QString Bridge::nextItemName(int sheet_index) const
{
    return QString::fromStdString(
            r.getTree().nextName(Graph::SheetIndex(sheet_index), "i"));
}

QString Bridge::nextSheetName(int sheet_index) const
{
    return QString::fromStdString(
            r.getTree().nextName(Graph::SheetIndex(sheet_index), "S"));
}

QObject* Bridge::singleton(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return singleton();
}

Bridge* Bridge::singleton()
{
    if (_instance == nullptr)
    {
        _instance = new Bridge();
    }
    return _instance;
}

void Bridge::attachCanvas(App::Render::Canvas* c)
{
    connect(this, &Bridge::push, c, &App::Render::Canvas::push);
    connect(this, &Bridge::pop, c, &App::Render::Canvas::pop);
    connect(this, &Bridge::cell, c, &App::Render::Canvas::cell);

    emit(syncLater());
}

void Bridge::setVariables(const Kernel::Solver::Solution& sol)
{
    {   // Roll out a set of variable updates
        auto lock = r.Lock();
        for (const auto& s : sol)
        {
            auto k = static_cast<Bind::CellKeyTag*>(
                    Kernel::Cache::instance()->tag(s.first))->key;

            std::stringstream ss;
            ss << s.second;
            r.setExprOrInput(k, ss.str());
        }
    }
    sync();
}

Graph::Root* Bridge::root()
{
    if (_instance == nullptr)
    {
        _instance = new Bridge();
    }
    return &_instance->r;
}

void Bridge::sync()
{
    assert(QObject::thread() == QThread::currentThread());
    r.getTree().serialize(&bts);
}

////////////////////////////////////////////////////////////////////////////////

Bridge::Checkpoint::Checkpoint(const QString& desc)
    : desc(desc), before(Bridge::root()->getTree().toString())
{
    // Nothing to do here
}

Bridge::Checkpoint::~Checkpoint()
{
    UndoStack::singleton()->push(new UndoCommand(
                desc, before, Bridge::root()->getTree().toString()));
    Bridge::singleton()->sync();
}

////////////////////////////////////////////////////////////////////////////////

bool Bridge::BridgeTreeSerializer::push(Graph::InstanceIndex i,
                                        const std::string& instance_name,
                                        const std::string& sheet_name)
{
    parent->pinged = false;
    parent->push(i.i, QString::fromStdString(instance_name),
                 QString::fromStdString(sheet_name));
    return parent->pinged;
}

void Bridge::BridgeTreeSerializer::pop()
{
    parent->pop();
}

void Bridge::BridgeTreeSerializer::instance(
        Graph::InstanceIndex i, const std::string& name,
        const std::string& sheet)
{
    parent->instance(i.i, QString::fromStdString(name),
            QString::fromStdString(sheet));
}

void Bridge::BridgeTreeSerializer::input(
        Graph::CellIndex c, const std::string& name,
        const std::string& expr, bool valid,
        const std::string& val)
{
    parent->input(c.i, QString::fromStdString(name),
                  QString::fromStdString(expr), valid,
                  QString::fromStdString(val));
}

void Bridge::BridgeTreeSerializer::output(
        Graph::CellIndex c, const std::string& name,
        bool valid, const std::string& val)
{
    parent->output(c.i, QString::fromStdString(name),
                   valid, QString::fromStdString(val));
}

void Bridge::BridgeTreeSerializer::cell(
        Graph::CellIndex c, const std::string& name,
        const std::string& expr, Graph::Cell::Type type,
        bool valid, const std::string& val, Graph::ValuePtr ptr)
{
    parent->cell(c.i, QString::fromStdString(name),
                 QString::fromStdString(expr), type,
                 valid, QString::fromStdString(val), ptr);
}

void Bridge::BridgeTreeSerializer::sheet(
        Graph::SheetIndex s, const std::string& name,
        bool editable, bool insertable)
{
    parent->sheet(s.i, QString::fromStdString(name), editable, insertable);
}

}   // namespace Core
}   // namespace App
