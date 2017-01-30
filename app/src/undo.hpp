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

    /*
     *  Stores the state of the graph in state
     */
    Q_INVOKABLE void mark(const QString& d);

    /*
     *  Pushes a new UndoCommand from state to current state
     */
    Q_INVOKABLE void finish();

protected:
    std::string state;
    QString desc;

    static UndoStack* _instance;
};

////////////////////////////////////////////////////////////////////////////////

class UndoCommand : public QUndoCommand
{
public:
    UndoCommand(const QString& desc, const std::string& before,
                const std::string& after);

    void undo() override;
    void redo() override;

protected:
    const std::string before;
    const std::string after;
};
