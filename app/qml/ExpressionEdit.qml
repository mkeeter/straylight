import QtQuick 2.7

import Bridge 1.0
import Style 1.0
import Material 1.0
import UndoStack 1.0

TextEdit {
    font.family: fixedWidth.name
    selectionColor: Style.textSelect
    selectByMouse: true
    color: expr.length ? Style.textDarkPrimary : Style.textDarkHint

    property string expr: ""
    property bool undoing: false
    property bool hasMatch: false

    // Catch undo / redo keys to use the global undo / redo stack
    Keys.onPressed: {
        if (event.matches(StandardKey.Undo)) {
            storeUndo()
            UndoStack.tryUndo()
            event.accepted = true
        } else if (event.matches(StandardKey.Redo)) {
            UndoStack.tryRedo()
            event.accepted = true
        }
    }

    // Rectangles to highlight matched parentheses
    Rectangle {
        id: matchA
        visible: parent.hasMatch && parent.activeFocus
        color: Material.lime
        opacity: 0.5
    }
    Rectangle {
        id: matchB
        visible: parent.hasMatch && parent.activeFocus
        color: Material.lime
        opacity: 0.5
    }

    Component.onCompleted: {
        syncText()
        Bridge.installHighlighter(textDocument)
    }
    onActiveFocusChanged: {
        syncText()
        // If we're losing focus, then store the undo command now
        // (rather than waiting two seconds for the timer to elapse)
        if (!activeFocus) {
            storeUndo()
        }
    }
    onExprChanged: syncText()

    onTextChanged: {
        if (activeFocus && !undoing) {
            undoTimer.restart()
            setExpr()
        }
        onCursorPositionChanged()
    }

    onCanUndoChanged: {
        if (canUndo) {
            UndoStack.mark("edit text")
        }
    }

    function storeUndo() {
        if (undoTimer.running) {
            undoTimer.stop()
        }
        if (canUndo)
        {
            // Store the actual undo command to the stack
            UndoStack.finish()

            // Store the final state so we can restore it
            var textAfter = text
            var c = cursorPosition

            // Here, we call undo just to clear out the undo stack
            // (because otherwise the onCanUndoChanged handler won't
            // be called ever again), then immediately restore text
            // and cursor position.
            undoing = true;
                undo()
                text = textAfter
            undoing = false;

            cursorPosition = Math.min(text.length, c)

            if (canUndo) {
                console.log("ERROR: too many undos in stack")
            }
        }
    }

    Timer {
        id: undoTimer
        interval: 2000
        onTriggered: {
            console.log(parent)
            parent.storeUndo()
        }
    }

    onCursorRectangleChanged: flick.scrollTo(cursorRectangle)
    onCursorPositionChanged: {
        var pos = Bridge.matchedParen(textDocument, cursorPosition)
        if (pos.x != -1 && pos.y != -1) {
            highlightPos(pos.x, matchA)
            highlightPos(pos.y, matchB)
            hasMatch = true;
        } else {
            hasMatch = false;
        }
    }

    /*
     *  Helper function that moves the given object to a particular
     *  position.  The object should be a semi-transparent Rectangle
     */
    function highlightPos(pos, obj) {
        var r = positionToRectangle(pos)

        obj.x = r.x
        obj.y = r.y
        obj.height = r.height
        obj.width = positionToRectangle(pos + 1).x - r.x
    }
}
