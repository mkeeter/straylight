#include "core/undo.hpp"
#include "core/bridge.hpp"

namespace App {
namespace Core {

UndoStack* UndoStack::_instance = nullptr;

QObject* UndoStack::singleton(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return singleton();
}

UndoStack* UndoStack::singleton()
{
    if (_instance == nullptr)
    {
        _instance = new UndoStack();
    }
    return _instance;
}

void UndoStack::tryUndo()
{
    if (canUndo())
    {
        undo();
    }
}

void UndoStack::tryRedo()
{
    if (canRedo())
    {
        redo();
    }
}

void UndoStack::mark(const QString& d)
{
    desc = d;
    state = Bridge::singleton()->root()->getTree().toString();
}

void UndoStack::finish()
{
    push(new UndoCommand(desc, state, Bridge::root()->getTree().toString()));
}

////////////////////////////////////////////////////////////////////////////////

UndoCommand::UndoCommand(const QString& desc,
                         const std::string& before,
                         const std::string& after)
    : QUndoCommand(desc), before(before), after(after)
{
    // Nothing to do here
}

void UndoCommand::undo()
{
    Bridge::singleton()->root()->loadString(before);
    Bridge::singleton()->sync();
}

void UndoCommand::redo()
{
    if (!once)
    {
        Bridge::singleton()->root()->loadString(after);
        Bridge::singleton()->sync();
    }
    once = false;
}

}   // namespace Core
}   // namespace App
