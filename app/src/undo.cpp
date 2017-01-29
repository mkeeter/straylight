#include "undo.hpp"
#include "bridge.hpp"

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

////////////////////////////////////////////////////////////////////////////////

UndoCommand::UndoCommand(Bridge& bridge, Graph::Root& root,
                         const QString& desc,
                         const std::string& before,
                         const std::string& after)
    : QUndoCommand(desc), bridge(bridge), root(root),
      before(before), after(after)
{
    // Nothing to do here
}

void UndoCommand::undo()
{
    root.loadString(before);
    bridge.sync();
}

void UndoCommand::redo()
{
    root.loadString(after);
    bridge.sync();
}
