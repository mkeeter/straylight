#pragma once

#include <QQuickTextDocument>
#include <QJSEngine>
#include <QUndoCommand>
#include <QUndoStack>

#include "graph/root.hpp"

class Bridge;

class UndoStack : public QUndoStack
{
public:
    /*
     *  Constructor for the QML singleton
     */
    static QObject* singleton(QQmlEngine *engine, QJSEngine *scriptEngine);
    static UndoStack* singleton();

protected:
    static UndoStack* _instance;
};

////////////////////////////////////////////////////////////////////////////////

class UndoCommand : public QUndoCommand
{
public:
    UndoCommand(Bridge& bridge, Graph::Root& root,
                const std::string& before, const std::string& after);

    void undo() override;
    void redo() override;

protected:
    Bridge& bridge;
    Graph::Root& root;

    const std::string before;
    const std::string after;
};
