import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0

import Style 1.0
import Bridge 1.0
import UndoStack 1.0
import Awesome 4.7
import Material 1.0

GridLayout {

    property color indicatorColor: valid ? Style.textEditValid : Style.textEditInvalid

    Rectangle {
        Layout.row: 0
        Layout.column: 0
        Layout.fillHeight: true
        width: 2
        color: indicatorColor
        opacity: exprText.lineCount > 1
        Behavior on opacity { OpacityAnimator { duration: 100 }}
    }

    function setFocus() {  exprText.forceActiveFocus() }

    Flickable {
        id: flick

        Layout.row: 0
        Layout.column: 1
        Layout.fillWidth: true
        Layout.preferredHeight: exprText.height
        clip: true

        function scrollTo(r) {
            if (contentX >= r.x) {
                contentX = r.x;
            } else if (contentX + width <= r.x + r.width) {
                contentX = r.x + r.width - width;
            }
        }

        TextEdit {
            color: expr.length ? Style.textDarkPrimary : Style.textDarkHint
            font.family: fixedWidth.name
            selectionColor: Style.textSelect
            selectByMouse: true

            id: exprText
            property string expr
            property bool hasMatch
            property bool undoing: false

            function syncText() {
                var c = cursorPosition
                text = activeFocus ? expr :
                    (ioType == 'input' ? '(input ...)' :
                        (expr.length ? expr : 'Expression'))
                cursorPosition = Math.min(text.length, c)
            }

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
                    exprText.storeUndo()
                }
            }

            onTextChanged: {
                if (activeFocus && !undoing) {
                    undoTimer.restart()
                    Bridge.setExpr(uniqueIndex, text)
                }
                onCursorPositionChanged()
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
    }

    Rectangle {
        Layout.row: 1
        Layout.column: 1
        Layout.fillWidth: true
        height: 2
        color: indicatorColor
        opacity: exprText.lineCount <= 1
        Behavior on opacity { OpacityAnimator { duration: 100 }}
    }

    Row {
        Layout.row: 2
        Layout.column: 1
        Layout.fillWidth: true

        visible: !(resultText.text.trim() === exprText.text.trim())
        Text {
            id: statusIcon
            text: valid ? Awesome.fa_long_arrow_right
                        : Awesome.fa_exclamation_triangle

            color: indicatorColor
            font.family: fontAwesome.name
            font.pointSize: 14

            rightPadding: 10
            anchors.verticalCenter: resultText.verticalCenter
        }

        Text {
            id: resultText
            wrapMode: Text.Wrap
            text: value
            width: parent.width - statusIcon.width

            font.family: fixedWidth.name
            color: Style.textDarkSecondary
        }
    }

    function setExpr(e)  {
        if (exprText.expr != e) {
            exprText.expr = e
        }
    }
}
