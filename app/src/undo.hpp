#pragma once

#include <QQuickTextDocument>
#include <QJSEngine>
#include <QUndoCommand>
#include <QUndoStack>

#include "graph/root.hpp"

class Bridge;

class UndoStack : public QUndoStack
{
    Q_OBJECT
public:
    /*
     *  Constructor for the QML singleton
     */
    static QObject* singleton(QQmlEngine *engine, QJSEngine *scriptEngine);
    static UndoStack* singleton();

    Q_INVOKABLE void tryUndo();
    Q_INVOKABLE void tryRedo();

protected:
    static UndoStack* _instance;
};

////////////////////////////////////////////////////////////////////////////////

class UndoCommand : public QUndoCommand
{
public:
    UndoCommand(Bridge& bridge, Graph::Root& root, const QString& desc,
                const std::string& before, const std::string& after);

    void undo() override;
    void redo() override;

protected:
    Bridge& bridge;
    Graph::Root& root;

    const std::string before;
    const std::string after;
};
